#pragma once

#include <exception>
#include <atomic>
#include <utility>

#include <iostream>

namespace common{

template<typename T>
class mpmc_bounded_queue
{
public:

    using item_type = T;
    mpmc_bounded_queue(size_t buffer_size)
        :max_size_(buffer_size),
         buffer_(new cell_t[buffer_size]),
         buffer_mask_(buffer_size - 1)
    {
        //queue size must be power of two
        if (!((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0)))
            throw "async logger queue size must be power of two";
        //throw spdlog_ex("async logger queue size must be power of two");

        for (size_t i = 0; i != buffer_size; i += 1)
            buffer_[i].sequence_.store(i, std::memory_order_relaxed);
        enqueue_pos_.store(0, std::memory_order_relaxed);
        dequeue_pos_.store(0, std::memory_order_relaxed);
    }

    ~mpmc_bounded_queue()
    {
        delete[] buffer_;
    }


    bool enqueue(T&& data)
    {
        cell_t* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_relaxed);
        for (;;)
        {
            cell = &buffer_[pos & buffer_mask_];
            size_t seq = cell->sequence_.load(std::memory_order_acquire);
            intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            if (dif == 0)
            {
                if (enqueue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    break;
            }
            else if (dif < 0)
            {
                return false;
            }
            else
            {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
        cell->data_ = std::move(data);
        cell->sequence_.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool dequeue(T& data)
    {
        cell_t* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);
        for (;;)
        {
            cell = &buffer_[pos & buffer_mask_];
            size_t seq =
                cell->sequence_.load(std::memory_order_acquire);
            intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            if (dif == 0)
            {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    break;
            }
            else if (dif < 0)
                return false;
            else
                pos = dequeue_pos_.load(std::memory_order_relaxed);
        }
        data = std::move(cell->data_);
        cell->sequence_.store(pos + buffer_mask_ + 1, std::memory_order_release);
        return true;
    }

    bool is_empty()
    {
        size_t front, front1, back;
        // try to take a consistent snapshot of front/tail.
        do
        {
            front = enqueue_pos_.load(std::memory_order_acquire);
            back = dequeue_pos_.load(std::memory_order_acquire);
            front1 = enqueue_pos_.load(std::memory_order_relaxed);
        }
        while (front != front1);
        return back == front;
    }


private:
    struct cell_t
    {
        std::atomic<size_t>   sequence_;
        T                     data_;
    };

    size_t const max_size_;

    static size_t const     cacheline_size = 64;
    typedef char            cacheline_pad_t[cacheline_size];

    cacheline_pad_t         pad0_;
    cell_t* const           buffer_;
    size_t const            buffer_mask_;
    cacheline_pad_t         pad1_;
    std::atomic<size_t>     enqueue_pos_;
    cacheline_pad_t         pad2_;
    std::atomic<size_t>     dequeue_pos_;
    cacheline_pad_t         pad3_;

    mpmc_bounded_queue(mpmc_bounded_queue const&) = delete;
    void operator= (mpmc_bounded_queue const&) = delete;
};

} // ns details

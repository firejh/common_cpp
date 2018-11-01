## unix socket
---

## reliable 

unix socket datagram 形式的包也是可靠的，每次写必然要求对应一次读，否则写方会被阻塞。如果是 stream 形式，则 buffer 没有满之前，写者是不会被阻塞的。datagram 的优势在于 api 简单。

```
Unix sockets are reliable. If the reader doesn't read, the writer blocks. If the socket is a datagram socket, each write is paired with a read. If the socket is a stream socket, the kernel may buffer some bytes between the writer and the reader, but when the buffer is full, the writer will block. Data is never discarded, except for buffered data if the reader closes the connection before reading the buffer.  ---[Do UNIX Domain Sockets Overflow?](https://unix.stackexchange.com/questions/283323/do-unix-domain-sockets-overflow)
```
```
On most UNIX implementations, UNIX domain datagram sockets are always reliable and don't reorder
       datagrams.   ---[man 7 socketpair](http://www.man7.org/linux/man-pages/man7/unix.7.html)
```

##  buffer size

datagram 形式的 unix socket 的单个 datagram 包最大长度是 130688 B。   


```
AF_UNIX SOCK_DATAGRAM/SOCK_SEQPACKET datagrams need contiguous memory. Contiguous physical memory is hard to find, and the allocation fails. The max size actually is 130688 B.  --- [the max size of AF_UNIX datagram message that can be sent in linux](https://stackoverflow.com/questions/4729315/what-is-the-max-size-of-af-unix-datagram-message-that-can-be-sent-in-linux)
```
```
It looks like AF_UNIX sockets don't support scatter/gather on current Linux. it is a fixed size 130688 B.                      --- [Difference between UNIX domain STREAM and DATAGRAM sockets?](https://stackoverflow.com/questions/13953912/difference-between-unix-domain-stream-and-datagram-sockets)

```

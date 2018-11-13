#include "server.h"


MetaServer::MetaServer()
{
    running_ = false;
}

MetaServer::~MetaServer()
{
}

void MetaServer::open()
{
    if (true == running_) return;

    running_ = true;



}

void MetaServer::close()
{
    running_ = false;
    /*
    for (auto th : threads_) {
        if (th->joinable()) {
            th->join();
        }
    }*/

    //stop
}

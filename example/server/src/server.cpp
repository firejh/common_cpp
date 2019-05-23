#include "server.h"


Server::Server()
{
    running_ = false;
}

Server::~Server()
{
}

void Server::open()
{
    if (true == running_) return;

    running_ = true;



}

void Server::close()
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

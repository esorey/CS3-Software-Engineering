#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H


#include <boost/asio.hpp>
#include "request.h"

using boost::asio::ip::tcp;

class request_handler {

    protected:
        tcp::socket *sock;
        request r;
};

#endif
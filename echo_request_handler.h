#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H
#include "request_handler.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class echo_request_handler: public request_handler {
    public:
        void handle() {
            boost::asio::write(*sock, boost::asio::buffer(r.data, r.length));
        }

        void set_request(request req) {
            r = req;
        }

        void set_socket(tcp::socket *s) {
            sock = s;
        }

        echo_request_handler(request req, tcp::socket *s);
};

echo_request_handler::echo_request_handler(request req, tcp::socket *s) {
    r = req;
    sock = s;
}


#endif
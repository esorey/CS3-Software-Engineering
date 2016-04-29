#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H
#include "request_handler.h"
#include "reply.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class echo_request_handler: public request_handler {
    public:
        void handle() {
            reply rep;
            for (int i = 0; i < r.length; i++) {
                rep.body.append(1, r.data[i]);
            }
            rep.status = "HTTP/1.0 200 OK";
            rep.content_type = "content-type: text/plain";
            serve_reply(rep);
        }

        void set_request(request req) {
            r = req;
        }

        void set_socket(tcp::socket *s) {
            sock = s;
        }

        echo_request_handler(request req, tcp::socket *s);
};

echo_request_handler::echo_request_handler(request req, tcp::socket *s) : request_handler(req, s) {
    r = req;
    sock = s;
}


#endif

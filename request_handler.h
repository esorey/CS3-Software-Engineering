#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H


#include <boost/asio.hpp>
#include "request.h"
#include "reply.h"

using boost::asio::ip::tcp;

class request_handler {

    public:
        void serve_reply(reply rep) {
            std::stringstream ss;
            ss << rep.status << std::endl;
            ss << rep.content_type << std::endl;
            ss << "content-length: " << rep.body.length() << std::endl << std::endl;
            ss << rep.body;
            boost::asio::write(*sock, boost::asio::buffer(ss.str()));
        }

        request_handler(request req, tcp::socket *s) {
            sock = s;
            r = req;
        }

    protected:
        tcp::socket *sock;
        request r;
};

#endif
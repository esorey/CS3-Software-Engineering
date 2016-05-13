#ifndef REQUEST_HANDLERR_H
#define REQUEST_HANDLERR_H


#include <boost/asio.hpp>
#include "request.h"
#include "reply.h"

using boost::asio::ip::tcp;

// Base request handler class
class request_handler {

    public:
        // servers a reply  to the requester
        void serve_reply(reply rep) {
        	std::string reply_string = build_reply_string(rep);
            boost::asio::write(*sock, boost::asio::buffer(reply_string));
        }

        // converts a reply struct into proper HTTP reply syntax
        std::string build_reply_string(reply rep) {
        	std::stringstream ss;
            ss << rep.status << std::endl;
            ss << rep.content_type << std::endl;
            ss << "content-length: " << rep.body.length() << std::endl << std::endl;
            ss << rep.body;
            return ss.str();
        }

        // class constructor
        request_handler(request req, tcp::socket *s) {
            sock = s;
            r = req;
        }

    protected:
        tcp::socket *sock; // the socket to read from and write to
        request r; // the incoming request
};

#endif
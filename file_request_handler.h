#ifndef FILE_REQUEST_HANDLER_H
#define FILE_REQUEST_HANDLER_H
#include "request_handler.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// File request handler subclass- handles requests that need files to be served
class file_request_handler: public request_handler {
    public:
        file_request_handler(request req, tcp::socket *s, std::string fp, std::string bp);
        void handle();
    protected:
        std::string filepath; // The relative path to the request file
        std::string basepath; // THe base path to the server file directory
};




#endif
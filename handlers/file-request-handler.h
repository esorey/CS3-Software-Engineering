#ifndef FILE_REQUEST_HANDLER_H
#define FILE_REQUEST_HANDLER_H

#include "request-handler.h"

class FileRequestHandler: public RequestHandler {
    public:

        bool Init(const std::map<std::string, std::string>& config_map);

        bool HandleRequest(const HttpRequest& request, HttpResponse* response);

    protected:
        std::string basepath;
};


#endif
#ifndef NOT_FOUND_REQUEST_HANDLER_H
#define NOT_FOUND_REQUEST_HANDLER_H

#include "request-handler.h"

class NotFoundRequestHandler: public RequestHandler {
public:
    bool Init(const std::map<std::string, std::string>& config_map);
    bool HandleRequest(const HttpRequest& request, HttpResponse* response);
};


#endif
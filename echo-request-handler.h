#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request-handler.h"

class EchoRequestHandler: public RequestHandler {
public:
    bool Init(const std::map<std::string, std::string>& config_map);
    bool HandleRequest(const HttpRequest& request, HttpResponse* response);
};


#endif
#ifndef PROXY_REQUEST_HANDLER_H
#define PROXY_REQUEST_HANDLER_H

#include "request-handler.h"

class ProxyRequestHandler: public RequestHandler {
    public:

        bool Init(const std::map<std::string, std::string>& config_map);

        bool HandleRequest(const HttpRequest& request, HttpResponse* response);

        void parseResponse(HttpResponse *response, std::string res);

    protected:
        std::string server_host;
        int server_port;
};


#endif

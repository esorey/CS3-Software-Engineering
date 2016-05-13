#include "echo-request-handler.h"

bool EchoRequestHandler::Init(const std::map<std::string, std::string>& config_map) {
    return true;
}

bool EchoRequestHandler::HandleRequest(const HttpRequest& request, HttpResponse* response) {
    response->http_version_ = "HTTP/1.0";
    response->status_code_ = "200";
    response->reason_phrase_ = "OK";
    response->content_type_ = "content-type: text/plain";
    response->body_ = request.raw_request_;

    return true;
}
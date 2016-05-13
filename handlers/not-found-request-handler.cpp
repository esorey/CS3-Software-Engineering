#include "not-found-request-handler.h"

bool NotFoundRequestHandler::Init(const std::map<std::string, std::string>& config_map) {
    return true;
}

bool NotFoundRequestHandler::HandleRequest(const HttpRequest& request, HttpResponse* response) {
    response->http_version_ = "HTTP/1.0";
    response->status_code_ = "404";
    response->reason_phrase_ = "Not Found";
    response->content_type_ = "content-type: text/plain";
    response->body_ = "Page not found";

    return true;
}
#include "file-request-handler.h"
#include "server_support.h"
#include <iostream>
#include <fstream>

bool FileRequestHandler::Init(const std::map<std::string, std::string>& config_map) {
    if(config_map.find("directory") == config_map.end()) {
        std::cerr << "Could not find directory info in handler\n";
        return false;
    }
    else {
        basepath = config_map.find("directory")->second;
        return true;
    }
}

bool FileRequestHandler::HandleRequest(const HttpRequest& request, HttpResponse* response) {
    std::string full_path = basepath + parse_filepath(request.raw_request_);
    full_path = full_path.substr(1);
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if(!is) {
        response->http_version_ = "HTTP/1.0";
        response->status_code_ = "404";
        response->reason_phrase_ = "Not Found";
        response->content_type_ = "content-type: text/html";
        response->body_ = "<html><body>404 File Not Found</body></html>";
        return true;
    }

    else {
        char buf[1024];
        while (is.read(buf, sizeof(buf)).gcount() > 0) {
            response->body_.append(buf, is.gcount());
        }
        response->http_version_ = "HTTP/1.0";
        response->status_code_ = "200";
        response->reason_phrase_ = "OK";
        response->content_type_ = parse_content_type(full_path);
        return true;
    }
}
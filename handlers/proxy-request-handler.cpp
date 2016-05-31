#include "proxy-request-handler.h"
#include "../server_support.h"
#include <iostream>
#include <fstream>


using boost::asio::ip::tcp;
using namespace std;

bool ProxyRequestHandler::Init(const std::map<std::string, std::string>& config_map) {
    if(config_map.find("server_host") == config_map.end()) {
        std::cerr << "Could not find server host info in handler\n";
        return false;
    }
    else {
        server_host = config_map.find("server_host")->second;
    }

    if(config_map.find("server_port") == config_map.end()) {
        std::cerr << "Could not find server port info in handler\n";
        return false;
    }
    else {
        server_host = stoi(config_map.find("server_port")->second);
    }

    return true;
}

bool ProxyRequestHandler::HandleRequest(const HttpRequest& request, HttpResponse* response) {
    response->http_version_ = request.version_;
    response->status_code_ = "200";
    response->reason_phrase_ = "OK";
}

void ProxyRequestHandler::parseResponse(HttpResponse *response, std::string res) {
    size_t start = res.find(" ");
    response->http_version_ = res.substr(0, start);

    size_t end = res.find(" ", start + 1);
    response->status_code_ = res.substr(start + 1, end - start - 1);

    start = end + 1;
    end = res.find("\r\n", start);
    response->reason_phrase_ = res.substr(start, end - start);

    // Parse headers
    while (res.find(":", end) != std::string::npos) {
        start = res.find("\r\n", start) + 2;
        end = res.find(":", start);
        std::string first = res.substr(start, end - start);

        start = end + 2;
        end = res.find("\r\n", start);
        std::string second = res.substr(start, end - start);

        response->headers_.push_back(std::pair<std::string, std::string>(first, second));
    }

    // body is found after an empty line after the headers
    response->body_ = res.substr(end + 4);
}

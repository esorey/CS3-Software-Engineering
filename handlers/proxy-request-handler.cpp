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

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
        server_port = stoi(config_map.find("server_port")->second);
    }

    return true;
}

bool ProxyRequestHandler::HandleRequest(const HttpRequest& request, HttpResponse* response) {
    // Parse incoming request -- Done in parseResponse
    HttpRequest newRequest = request;
    
    // Modify request by substituting in proxy host
    for (int i = 0; i < newRequest.headers_.size(); i++) {
        std::pair<std::string, std::string> head = newRequest.headers_.at(i);
        if (head.first == "Host") {
            newRequest.headers_.at(i).second = server_host;
        }
        else if (head.first == "Connection") {
            newRequest.headers_.at(i).second = "close";
        }
        else if (head.first == "Accept-Encoding") {
            newRequest.headers_.at(i).second = "*";
        }
    }
    // 6 is length of the text "/proxy"
    newRequest.uri_ = newRequest.uri_.substr(6);
    if (newRequest.uri_ == "") {
        newRequest.uri_ = "/";
    }
    cout << newRequest.toString() << endl;
    
    // Connect to different HTTP server and issue modified request
    // Read response from server
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(server_host, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if (error) {
        throw boost::system::system_error(error);
    }

    // From Boost docs
    // <http://www.boost.org/doc/libs/1_38_0/doc/html/boost_asio/example/socks4/sync_client.cpp>
    socket.send(boost::asio::buffer(newRequest.toString()));
    std::string r;
    do {
        char buf[1024];
        size_t bytes_transferred =
            socket.receive(boost::asio::buffer(buf), {}, error);
        if (!error) r.append(buf, bytes_transferred);
    } while (!error);
    
    parseResponse(response, r);
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
        
        // Make necessary modifications to response
        if (first == "Connection") {
            second = "close";
        }

        response->headers_.push_back(std::pair<std::string, std::string>(first, second));
        
        if (first == "Content-Type") {
            response->content_type_ = second;
        }
    }

    // body is found after an empty line after the headers
    response->body_ = res.substr(end + 4);
}

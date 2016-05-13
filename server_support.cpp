#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"
#include "request-handler.h"


using boost::asio::ip::tcp;

const int max_length = 1024;

// Parses the content type of a request string
std::string parse_content_type(std::string filepath) {
    std::string::size_type dot_pos = filepath.find('.');
    if (dot_pos == std::string::npos) {
        printf("could not find dot in filetype string \n");
        return NULL;
    }

    std::string ext = filepath.substr(dot_pos + 1);

    std::string s = "content-type: ";
    if (ext.compare("png") == 0) {
        s += "image/png";
    }
    else if (ext.compare("jpg") == 0) {
        s += "image/jpeg";
    }

    else if (ext.compare("html") == 0) {
        s += "text/html";
    }

    else if (ext.compare("txt") == 0) {
        s += "text/plain";
    }
    else {
        return NULL;
    }
    // TODO: Support more extensions

    return s;
}

// Parses the valid request paths for each request handler from the config file
std::vector<std::string> parse_request_paths(const char *config_string, int line) {
    std::vector<std::string> ret;
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    // TODO error-check the first token to make sure it is valid

    // Build a vector of valid request prefix string
    for (int i = 1; i < config.statements_.at(line)->tokens_.size(); i++) {
        ret.push_back(config.statements_.at(line)->tokens_.at(i));
    }
    return ret;
}

// Parses the port number from a config file
int parse_port(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    if (config.statements_.at(PORT_LOCATION)->tokens_.at(0).compare("port") == 0) {
        return std::stoi(config.statements_.at(PORT_LOCATION)->tokens_.at(1));
    }
    else {
        std::cerr << "Could not find port in config file" << std::endl;
        return -1; 
    }
}

// Parses the base file directory path from the config file
std::string parse_base_path(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    if (config.statements_.at(BASEPATH_LOCATION)->tokens_.at(0).compare("base_path") != 0) {
        std::cerr << "invalid config file format" << std::endl;
        return NULL;
    }
    return config.statements_.at(BASEPATH_LOCATION)->tokens_.at(1); 
}

// Parses a prefix from an HTTP request.
std::string parse_request_prefix(std::string request) {

    // Find the first slash
    std::string::size_type slash_pos1 = request.find('/');
    if (slash_pos1 == std::string::npos) {
        printf("Improper HTTP request format \n");
        return NULL;
    }

    // Find the next slash and next space
    std::string::size_type slash_pos2 = request.find('/', slash_pos1 + 1);
    std::string::size_type space_pos = request.find(' ', slash_pos1);

    // We want the substring from the first slash to the next slash or space,
    // whichever comes first.
    return request.substr(slash_pos1, std::min(space_pos, slash_pos2) - slash_pos1);
}

// Parses the filepath from a request with a file prefix
std::string parse_filepath(const char *data) {
    std::string request(data);

    // Finds the substring between the first slash and the next space
    std::string::size_type slash_pos1 = request.find('/');
    if (slash_pos1 == std::string::npos) {
        printf("Improper request format \n");
        return NULL;
    }

    std::string::size_type slash_pos2 = request.find('/', slash_pos1 + 1);
    if (slash_pos2 == std::string::npos) {
        printf("Improper request format \n");
        return NULL;
    }

    std::string::size_type space_pos = request.find(' ', slash_pos2);
    if (slash_pos2 == std::string::npos) {
        printf("Improper request format \n");
        return NULL;
    }
    return request.substr(slash_pos2, space_pos - slash_pos2);
}

std::string build_reply_string(HttpResponse* resp) {
    std::stringstream ss;
    ss << resp->http_version_ << " " << resp->status_code_ << " " << resp->reason_phrase_ << std::endl;
    ss << resp->content_type_ << std::endl;
    ss << "content-length: " << resp->body_.length() << std::endl << std::endl;
    ss << resp->body_;
    return ss.str();
}

// TODO: This should really be a class method for some sort of request
// parser object. That way we can store the config parameters instead
// of having to recompute them for every request

// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, const std::map<std::string, RequestHandler*>& handler_map) {
    try {
        // build request by reading in request data in from tcp socket
        char data[1024];
        HttpRequest req;
        boost::system::error_code error;
        int length = sock->read_some(boost::asio::buffer(data), error);

        for (int i = 0; i < length; i++) {
            req.raw_request_.append(1, data[i]);
        }

        // handle errors when reading in the request
        if (error == boost::asio::error::eof) {
            printf("EOF error \n");
            return;
        }
        else if (error) {
            printf("Some other error \n");
            throw boost::system::system_error(error);
        }
        
        // Grab the request prefix
        std::string prefix = parse_request_prefix(req.raw_request_);
        HttpResponse resp;
        std::cout << prefix << std::endl;
        if (handler_map.find(prefix) != handler_map.end()) {
            RequestHandler* h = handler_map.find(prefix)->second;
            h->HandleRequest(req, &resp);
        }

        else { // Couldn't find handler; bad request
            resp.http_version_ = "HTTP/1.0";
            resp.status_code_ = "400";
            resp.reason_phrase_ = "Bad Request";
            resp.content_type_ = "content-type text/html";
            resp.body_ = "<html><body>400 Bad Request</body></html>";  
        }

        std::string reply = build_reply_string(&resp);
        boost::asio::write(*sock, boost::asio::buffer(reply));
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
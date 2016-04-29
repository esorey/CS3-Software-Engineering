#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"
#include "request.h"
#include "echo_request_handler.h"
#include "file_request_handler.h"

using boost::asio::ip::tcp;

const int max_length = 1024;


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
        s += "image/jpg";
    }

    else if (ext.compare("html") == 0) {
        s += "text/html";
    }

    else {
        return NULL;
    }

    return s;
}

// Serves a simple hello world HTML page
std::string serve_hello_page() {
    std::stringstream ss;
    std::string body = "<html><body>Hello, world!</body></html>";
    ss << "HTTP/1.0 200 OK\n";
    ss << "content-type: text/html\n";
    ss << "content-length: " << body.length() << "\n\n";
    ss << body;
    return ss.str();
}

// Parses a port number from a config file
int parse_port(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    // iterate through config file looking for port
    for (int i = 0; i < config.statements_.size(); i++) {
        if (config.statements_.at(i)->tokens_.at(0).compare("port") == 0) {
            return std::stoi(config.statements_.at(i)->tokens_.at(1));
        }
    }

    std::cerr << "Could not find port in config file" << std::endl;
    return -1; 
}

// Parses a base path for static files from a config file
std::string parse_base_path(const char *config_string) {
    NginxConfigParser parser;
    NginxConfig config;
    std::stringstream config_stream(config_string);
    parser.Parse(config_string, &config);

    if (config.statements_.at(4)->tokens_.at(0).compare("base_path") != 0) {
        std::cerr << "invalid config file format" << std::endl;
        exit(1);
    }
    return config.statements_.at(4)->tokens_.at(1); 
}

// Parses a prefix from an HTTP request.
std::string parse_request_prefix(const char *data) {
    std::string request(data);
    // Find the first slash
    std::string::size_type slash_pos1 = request.find('/');
    if (slash_pos1 == std::string::npos) {
        printf("Improper HTTP request format \n");
        exit(1);
    }

    // Find the next slash and next space
    std::string::size_type slash_pos2 = request.find('/', slash_pos1 + 1);
    std::string::size_type space_pos = request.find(' ', slash_pos1);

    // We want the substring from the first slash to the next slash or space,
    // whichever comes first.
    return request.substr(slash_pos1, std::min(space_pos, slash_pos2) - slash_pos1);
}

std::string parse_filepath(const char *data) {
    std::string request(data);

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

// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, const char *config) {
    try {
        request r;
        boost::system::error_code error;
        r.length = sock->read_some(boost::asio::buffer(r.data), error);

        if (error == boost::asio::error::eof) {
            printf("EOF error \n");
            return;
        }

        else if (error) {
            printf("Some other error \n");
            throw boost::system::system_error(error);
        }
        
        // Grab the request prefix
        std::string prefix = parse_request_prefix(r.data);

        if (prefix.compare("/echo") == 0) {
            echo_request_handler e(r, sock);
            e.handle();
        }
        else if (prefix.compare("/static") == 0) {
            file_request_handler f(r, sock, parse_filepath(r.data), parse_base_path(config));
            f.handle();
        }
        else {
            request_handler h(r, sock);
            reply r;
            r.body = "<html><body>400 Bad Request</body></html>";
            r.status = "HTTP/1.0 400 Bad Request";
            r.content_type = "content-type text/html";
            h.serve_reply(r);   
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
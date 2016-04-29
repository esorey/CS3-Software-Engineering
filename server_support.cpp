#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"
#include "request.h"
#include "echo_request_handler.h"

using boost::asio::ip::tcp;

const int max_length = 1024;

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

    if (config.statements_.at(0)->tokens_.at(0).compare("port") != 0) {
        std::cerr << "first line of config file must be port number" << std::endl;
        return -1;
    }
    return std::stoi(config.statements_.at(0)->tokens_.at(1));
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
        printf("Improper HTTP request format");
        exit(1);
    }

    // Find the next slash and next space
    std::string::size_type slash_pos2 = request.find('/', slash_pos1 + 1);
    std::string::size_type space_pos = request.find(' ', slash_pos1);

    // We want the substring from the first slash to the next slash or space,
    // whichever comes first.
    return request.substr(slash_pos1, std::min(space_pos, slash_pos2) - slash_pos1);

}

// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, bool echo) {
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

        switch(prefix) {
            case "/echo": // Echo the HTTP request
                echo_request_handler e(r, sock);
                e.handle();

            case "/static": // Retrieve static pages
                // TODO: add static handler here


            default: // Serve "Hello, world!"
                boost::asio::write(*sock, boost::asio::buffer(serve_hello_page()));
                
        }
        /*if (echo) {  // Echo the HTTP request
            echo_request_handler e(r, sock);
            e.handle();
        }

        else {  // Serve "Hello, world!"
            boost::asio::write(*sock, boost::asio::buffer(serve_hello_page()));
        }*/
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
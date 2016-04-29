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
std::string parse_request_prefix(const char *data) {
    std::string request(data);
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

// TODO: This should really be a class method for some sort of request
// parser object. That way we can store the config parameters instead
// of having to recompute them for every request

// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, const char *config) {
    try {
        // build request by reading in request data in from tcp socket
        request r;
        boost::system::error_code error;
        r.length = sock->read_some(boost::asio::buffer(r.data), error);

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
        std::string prefix = parse_request_prefix(r.data);

        // TODO: store these results in some request parser object
        // Instead of having to compute them for every request
        std::vector<std::string> echo_prefixes = parse_request_paths(config, ECHO_LOCATION);
        std::vector<std::string> file_prefixes = parse_request_paths(config, FILE_LOCATION);

        // Match the request prefix to the appropriate handler
        if (std::find(echo_prefixes.begin(), echo_prefixes.end(), prefix) != echo_prefixes.end()) {
            echo_request_handler e(r, sock);
            e.handle();
        }
        else if (std::find(file_prefixes.begin(), file_prefixes.end(), prefix) != file_prefixes.end()) {
            file_request_handler f(r, sock, parse_filepath(r.data), parse_base_path(config));
            f.handle();
        }
        else { // Couldn't find handler; bad request
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
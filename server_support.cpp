#include <iostream>
#include <string>
#include <algorithm>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server_support.h"
#include "handlers/request-handler.h"
#include "handlers/echo-request-handler.h"
#include "handlers/file-request-handler.h"
#include "handlers/proxy-request-handler.h"
#include "handlers/not-found-request-handler.h"

using boost::asio::ip::tcp;

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
std::string parse_filepath(std::string request) {

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

void parse_request(HttpRequest &request) {
    std::string r = request.raw_request_;
    
    size_t start = r.find(" ");
    request.method_ = r.substr(0, start);

    size_t end = r.find(" ", start + 1);
    request.uri_ = r.substr(start + 1, end - start - 1);

    start = r.find("HTTP/");
    end = r.find("\r\n", start);
    request.version_ = r.substr(start, end - start);

    // Parse headers
    while (r.find(":", end) != std::string::npos) {
        start = r.find("\r\n", start) + 2;
        end = r.find(":", start);
        std::string first = r.substr(start, end - start);

        start = end + 2;
        end = r.find("\r\n", start);
        std::string second = r.substr(start, end - start);

        request.headers_.push_back(std::pair<std::string, std::string>(first, second));
    }

    // body is found after an empty line after the headers
    request.body_ = r.substr(end + 4);
}


// Handles incoming server requests according to provided parameters
// If echo is true, the server will echo the HTTP request it receives.
// Otherwise, it will serve "Hello, world!".
void handle_request(tcp::socket *sock, const std::map<std::string, std::shared_ptr<RequestHandler>>& handler_map) {
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

        parse_request(req);
        
        // Find the handler that corresponds to the request prefix
        std::string prefix = parse_request_prefix(req.raw_request_);
        HttpResponse resp;
        if (handler_map.find(prefix) != handler_map.end()) {
            std::shared_ptr<RequestHandler> h = handler_map.find(prefix)->second;
            h->HandleRequest(req, &resp);
        }

        else { // Couldn't find handler; bad request
            resp.http_version_ = "HTTP/1.0";
            resp.status_code_ = "400";
            resp.reason_phrase_ = "Bad Request";
            resp.content_type_ = "content-type text/html";
            resp.body_ = "<html><body>400 Bad Request</body></html>";  
        }

        std::string reply = resp.toString();
        boost::asio::write(*sock, boost::asio::buffer(reply));
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// Parses the config file into a map of request handlers
// Each request handler is initialized with the config info for that handler
void parse_config(int* port, std::map<std::string, std::shared_ptr<RequestHandler>>* handlers, const char* config_file) {
    NginxConfigParser parser;
    NginxConfig config;
    parser.Parse(config_file, &config);

    // iterate through each line of the config file
    for (int i = 0; i < config.statements_.size(); i++) {
        std::string type = config.statements_.at(i)->tokens_.at(0);

        // parses the port, making sure there is only one port statement in the file
        if (type.compare("port") == 0) {
            if (*port == -1) {
                *port = std::stoi(config.statements_.at(0)->tokens_.at(1));
            }
            else {
                std::cerr << "Duplicate port declaration!" << std::endl;
                exit(1);
            }
        }

        else if (type.compare("handler") == 0) {
            std::string handler_type = config.statements_.at(i)->tokens_.at(1);
            std::map<std::string, std::string> config_map;

            // iterate through the config statements and add them to the map for that handler
            for (int j = 0; j < config.statements_.at(i)->child_block_->statements_.size(); j++) {
                config_map.insert(std::pair<std::string, std::string>(config.statements_.at(i)->child_block_->statements_.at(j)->tokens_.at(0),
                                  config.statements_.at(i)->child_block_->statements_.at(j)->tokens_.at(1)));
            }

            // then create the appropriate handler based on the type string
            if (handler_type.compare("static") == 0) {
                std::shared_ptr<RequestHandler> f(new FileRequestHandler);
                f->Init(config_map);
                handlers->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(config_map["path"], f));
            }

            else if (handler_type.compare("echo") == 0) {
                std::shared_ptr<RequestHandler> e(new EchoRequestHandler);
                e->Init(config_map);
                handlers->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(config_map["path"], e));
            }

            else if (handler_type.compare("proxy") == 0) {
                std::shared_ptr<RequestHandler> p(new ProxyRequestHandler);
                p->Init(config_map);
                handlers->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(config_map["path"], p));
            }

            else if (handler_type.compare("notfound") == 0) {
                std::shared_ptr<RequestHandler> nf(new NotFoundRequestHandler);
                nf->Init(config_map);
                handlers->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(config_map["path"], nf));
            }
        }

        // right now we only support ports and handlers
        else {
            std::cerr << "Unsupported type: " << type << std::endl;
            exit(1);
        }
    }
}



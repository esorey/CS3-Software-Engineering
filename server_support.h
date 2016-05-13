#ifndef SERVER_SUPPORT_H
#define SERVER_SUPPORT_H

#define PORT_LOCATION 0
#define HELLO_LOCATION 1
#define ECHO_LOCATION 2
#define FILE_LOCATION 3
#define BASEPATH_LOCATION 4

#include <iostream>
#include <string>
#include "request-handler.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string parse_request_prefix(const char *data);

std::string parse_content_type(std::string filepath);

void handle_request(tcp::socket *sock, const std::map<std::string, std::shared_ptr<RequestHandler>>& handler_map);

std::string parse_filepath(std::string request);

void parse_config(int* port, std::map<std::string, std::shared_ptr<RequestHandler>>* handlers, const char* config_file);

#endif
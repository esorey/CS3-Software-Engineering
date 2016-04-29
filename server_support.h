#ifndef SERVER_SUPPORT_H
#define SERVER_SUPPORT_H

#define PORT_LOCATION 0
#define HELLO_LOCATION 1
#define ECHO_LOCATION 2
#define FILE_LOCATION 3
#define BASEPATH_LOCATION 4

#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int parse_port(const char *config_string);

std::string parse_base_path(const char *config_string);

std::string parse_request_prefix(const char *data);

std::string parse_content_type(std::string filepath);

void handle_request(tcp::socket *sock, const char *config);

#endif
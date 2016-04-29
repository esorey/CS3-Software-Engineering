#ifndef SERVER_SUPPORT_H
#define SERVER_SUPPORT_H

#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string serve_hello_page();

int parse_port(const char *config_string);

std::string parse_base_path(const char *config_string);

std::string parse_request_prefix(const char *data);

std::string parse_content_type(std::string filepath);

void handle_request(tcp::socket *sock, const char *config);

#endif
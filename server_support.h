#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string serve_hello_page();

int parse_port(const char *config_string);

std::string parse_base_path(const char *config_string);

std::string parse_request_prefix(const char *data);

void handle_request(tcp::socket *sock, bool echo);
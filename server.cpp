//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "config_parser.h"

using boost::asio::ip::tcp;

std::string make_hello_world_string()
{
  std::stringstream ssOut;
  std::string sHTML = "<html><body>Hello, world!</body></html>";
  ssOut << "HTTP/1.0 200 OK" << std::endl;
  ssOut << "content-type: text/html" << std::endl;
  ssOut << "content-length: " << sHTML.length() << std::endl;
  ssOut << std::endl;
  ssOut << sHTML;
  return ssOut.str();
}

int parse_port(const char *config_string) {
  NginxConfigParser parser;
  NginxConfig config;
  std::stringstream config_stream(config_string);
  parser.Parse(config_string, &config);
  return std::stoi(config.statements_.at(0)->tokens_.at(0));
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;
    std::cout << argv[1] << std::endl;
    std::string config_file = std::string(argv[1]);
    int port = parse_port(argv[1]);
    std::cout << port << std::endl;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

    for (;;)
    {
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      std::cout << "one \n";

      std::string message = make_hello_world_string();

      boost::system::error_code ignored_error;
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      std::cout << "two \n";
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
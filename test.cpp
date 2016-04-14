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

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

int main()
{
  try
  {
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8087));

    for (;;)
    {
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      std::cout << "one \n";


      std::stringstream ssOut;
      std::string sHTML = "<html><body>Hello, world!</body></html>";
      ssOut << "HTTP/1.0 200 OK" << std::endl;
      ssOut << "content-type: text/html" << std::endl;
      ssOut << "content-length: " << sHTML.length() << std::endl;
      ssOut << std::endl;
      ssOut << sHTML;
      std::string message = ssOut.str();

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
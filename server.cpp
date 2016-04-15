// David Inglis & Eli Sorey
// server.cpp: a very simple http server
// Referenced the Boost TCP server tutorial: 
// http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/tutorial/tutdaytime2.html

#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// Serves a simple hello world HTML page
std::string serve_page() {
    std::stringstream ss;
    std::string body = "<html><body>Hello, world!</body></html>";
    ss << "HTTP/1.0 200 OK\n";
    ss << "content-type: text/html\n";
    ss << "content-length: " << body.length() << "\n\n";
    ss << body;
    return ss.str();
}

int main() {
    // Try to create a tcp endpoint
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8087));

        // loop on accepting input and serving the page in response
        while(true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            boost::asio::write(socket, boost::asio::buffer(serve_page()));
        }
    }

    // Catch exception if we fail to create the endpoint
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
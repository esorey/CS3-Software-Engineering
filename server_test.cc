#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "server_support.h"
#include "request_handler.h"
#include "reply.h"

// TODO: end-to-end integration test of the server

TEST(ServerTest, PortParsing) {
    int port = parse_port("server_config");
    EXPECT_EQ(port, 8080);
}

TEST(ServerTest, BasePathParsing) {
	std::string base_path = parse_base_path("server_config_full");
	EXPECT_EQ(base_path, "files");
}

TEST(ServerTest, RequestPrefixParsing) {
	std::string prefix = parse_request_prefix("GET /echo HTTP/1.1");
	EXPECT_EQ(prefix, "/echo");

	prefix = parse_request_prefix("GET /static/test.html HTTP/1.1");
	EXPECT_EQ(prefix, "/static");

	prefix = parse_request_prefix("GET /simple HTTP/1.1");
	EXPECT_EQ(prefix, "/simple");
}

TEST(ServerTest, ReplyString) {
	reply rep;
	request req;
	rep.status = "HTTP/1.0 200 OK";
	rep.content_type = "content-type: text/plain";
	rep.body = "this is the reply body";
	request_handler *handler = new request_handler(req, NULL);
	std::string rep_string = handler->build_reply_string(rep);
	EXPECT_EQ(rep_string, "HTTP/1.0 200 OK\ncontent-type: text/plain\ncontent-length: 22\n\nthis is the reply body");
}
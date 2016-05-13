#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "server_support.h"
#include "handlers/request-handler.h"
#include "handlers/not-found-request-handler.h"
#include "handlers/echo-request-handler.h"

// TODO: end-to-end integration test of the server

/*TEST(ServerTest, PortParsing) {
    int port = parse_port("server_config");
    EXPECT_EQ(port, 8080);
}/*

/*TEST(ServerTest, BasePathParsing) {
	std::string base_path = parse_filepath("new_config");
	EXPECT_EQ(base_path, "files");
}/*

TEST(ServerTest, RequestPrefixParsing) {
	std::string prefix = parse_request_prefix("GET /echo HTTP/1.1");
	EXPECT_EQ(prefix, "/echo");

	prefix = parse_request_prefix("GET /static/test.html HTTP/1.1");
	EXPECT_EQ(prefix, "/static");

	prefix = parse_request_prefix("GET /simple HTTP/1.1");
	EXPECT_EQ(prefix, "/simple");
}

/*TEST(ServerTest, ReplyString) {
	reply rep;
	request req;
	rep.status = "HTTP/1.0 200 OK";
	rep.content_type = "content-type: text/plain";
	rep.body = "this is the reply body";
	request_handler *handler = new request_handler(req, NULL);
	std::string rep_string = handler->build_reply_string(rep);
	EXPECT_EQ(rep_string, "HTTP/1.0 200 OK\ncontent-type: text/plain\ncontent-length: 22\n\nthis is the reply body");
}*/

TEST(HandlerTest, NotFoundRequest) {
	HttpRequest req;
	HttpResponse resp;
	NotFoundRequestHandler *nfHandler = new NotFoundRequestHandler;
	nfHandler->HandleRequest(req, &resp);
	EXPECT_EQ(resp.http_version_, "HTTP/1.0");
	EXPECT_EQ(resp.status_code_, "404");
	EXPECT_EQ(resp.reason_phrase_, "Not Found");
	EXPECT_EQ(resp.content_type_, "content-type: text/plain");
	EXPECT_EQ(resp.body_, "Page not found");
}

TEST(HandlerTest, EchoRequest) {
	HttpRequest req;
	HttpResponse resp;
	EchoRequestHandler *echoHandler = new EchoRequestHandler;
	req.raw_request_ = "testing";
	echoHandler->HandleRequest(req, &resp);
	EXPECT_EQ(resp.http_version_, "HTTP/1.0");
	EXPECT_EQ(resp.status_code_, "200");
	EXPECT_EQ(resp.reason_phrase_, "OK");
	EXPECT_EQ(resp.content_type_, "content-type: text/plain");
	EXPECT_EQ(resp.body_, "testing");
}
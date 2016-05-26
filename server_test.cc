#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "server_support.h"
#include "handlers/request-handler.h"
#include "handlers/not-found-request-handler.h"
#include "handlers/echo-request-handler.h"

// TODO: end-to-end integration test of the server

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
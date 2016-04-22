#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "gtest/gtest.h"
#include "server_support.h"

TEST(ServerTest, PortParsing) {
    int port = parse_port("server_config");
    EXPECT_EQ(port, 8080);
}
#include "file_request_handler.h"
#include <iostream>
#include <string>
#include <fstream>

file_request_handler::file_request_handler(request req, 
                                           tcp::socket *s,
                                           std::string fp) : request_handler(req, s) {
    filepath = fp;
}

void file_request_handler::handle() {
    reply r;

    //std::string full_path = parse_base_path() + filepath;
    std::string full_path = "files" + filepath;

    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        r.body = "<html><body>404 Not Found</body></html>";
        r.status = "HTTP/1.0 404 Not Found";
        r.content_type = "content-type: text/html";
        serve_reply(r);
        return;
    }
    else {

    }
    char buf[1024];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        r.body.append(buf, is.gcount());
    }
    r.status = "HTTP/1.0 200 OK";
    r.content_type = "content-type: image/png";
    serve_reply(r);

}
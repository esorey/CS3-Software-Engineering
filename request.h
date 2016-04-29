#ifndef REQUEST_H
#define REQUEST_H
struct request {
    char data[1024];
    size_t length;
};
#endif
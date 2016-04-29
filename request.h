#ifndef REQUEST_H
#define REQUEST_H

// Simple request struct
struct request {
    char data[1024]; // buffer to store request data in
    size_t length; // number of characters in the request
};
#endif
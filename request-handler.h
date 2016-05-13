#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

using Headers = std::vector<std::pair<std::string, std::string>>;

struct HttpRequest {
    std::string raw_request_;

    std::string method_;
    std::string uri_;
    std::string version_;

    Headers headers_;

    std::string body_;
};

struct HttpResponse {
    std::string http_version_;
    std::string status_code_;
    std::string reason_phrase_;
    std::string content_type_;

    Headers headers_;

    std::string body_;
};

// Created a server startup, and lasts for the lifetime of the server.
class RequestHandler {
 public:
    // Initializes the RequestHandler from a map of configuration keys and values.
    // Returns true if the RequestHandler was initialized successfully. If the
    // return value is false, the object is invalid and should not be used.
    //
    // The config_map is the keys-value pairs in the config file.
    //
    // For example, given:
    // handler static {
    //   path /static;
    //   directory /foo;
    // }
    // The config_map would have keys "path" and "directory" with values "/static" and "/foo".
    virtual bool Init(const std::map<std::string, std::string>& config_map) = 0;
    
    // Handles an HTTP request. Writes the response to the HttpResponse object,
    // and returns true if the request was handled successfully. If the return value
    // is false, then the HttpResponse object is invalid and should not be used.
    //
    // Each config block has a special config named "path" that determines
    // the path prefix for which the handler will be called.
    //
    // For example, the config "handler echo { path /echo; }" means that an echo
    // handler will be called for all requests whose URI paths start with "/echo".
    //
    // The dispatch mechanism is implemented in the main server code.
    virtual bool HandleRequest(const HttpRequest& request, HttpResponse* response) = 0;
};
#endif
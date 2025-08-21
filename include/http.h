#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <stdbool.h>

#define HTTP_REQUEST_MAX_LEN 8192 * 4
#define HTTP_METHOD_MAX_LEN 8
#define HTTP_PATH_MAX_LEN 2048
#define HTTP_PROTOCOL_MAX_LEN 16

#define HTTP_MAX_HEADER_KEY_LEN 256
#define HTTP_MAX_HEADER_VALUE_LEN 256

typedef enum {
  HTTP_PARSE_OK,
  HTTP_PARSE_INVALID,
} http_parse_e;

typedef enum { GET, POST, PUT, DELETE } http_method_e;

typedef struct {
  char key[HTTP_MAX_HEADER_KEY_LEN];
  char value[HTTP_MAX_HEADER_VALUE_LEN];
} http_header_t;

typedef struct {
  char method[HTTP_METHOD_MAX_LEN];
  char path[HTTP_PATH_MAX_LEN];
  char protocol[HTTP_PROTOCOL_MAX_LEN];
  char buffer[HTTP_REQUEST_MAX_LEN];
} http_request_raw;

typedef struct {
  http_method_e method;
  char path[HTTP_PATH_MAX_LEN];
  char protocol[HTTP_PROTOCOL_MAX_LEN];
  http_header_t *headers; // heap-allocated array
  int header_count;
  char body[HTTP_REQUEST_MAX_LEN];
} http_request;

typedef struct {
  int status_code;        // Numeric status code (e.g., 200, 404)
  char reason_phrase[64]; // Text explanation of the status (e.g., "OK", "Not
                          // Found")
  http_header_t *headers; // Array of HTTP headers (key-value pairs)
  size_t header_count;    // Number of headers
  char *body;             // Response body content
  size_t body_length;     // Length of the response body
} http_response;

http_parse_e read_http_request(int socket_fd, http_request_raw *request);
http_parse_e parse_http_request(http_request_raw *raw_request, http_request *request);
void init_http_response(http_response *response);
void add_http_header(http_response *response, const char *key,
                     const char *value);
void set_http_body(http_response *response, const char *body);
char *construct_http_response(const http_response *response,
                              size_t *response_length);
void serve_file(const char *path, http_response *response);
void send_http_response(int client_fd, const http_response *response);
void sanitize_path(const char *requested_path, char *sanitized_path,
                   size_t buffer_size);
bool handle_request(http_request *req, http_response *res);
void free_http_headers(http_request *request);
void free_http_response(http_response *response);

#endif

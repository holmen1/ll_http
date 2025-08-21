#include "common.h"
#include "http.h"
#include "route.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>

#include <cjson/cJSON.h>

typedef struct {
  int port;
} server_config;

int loadConfig(server_config *config) {
  int status = 0;

  char *configdata = loadfile("config.json");
  if (!configdata) {
    debug_log("Failed to load config, what happened?");
  }

  cJSON *config_json = cJSON_Parse(configdata);
  if (config_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    status = 0;
    goto end;
  }

  cJSON *port = cJSON_GetObjectItemCaseSensitive(config_json, "portnumber");
  if (!cJSON_IsNumber(port)) {
    status = 0;
    goto end;
  }

  if (port->valueint > 65535 || port->valueint < 0) {
    debug_log("Invalid port number specified in config.");
    status = 0;
    goto end;
  }

  config->port = (short)port->valueint;

end:
  cJSON_Delete(config_json);
  return status;
}

void handle_client(int client_fd) {
  size_t buffer_size = 1024;
  http_request_raw req_raw = {0};
  http_request req = {0};
  http_response res = {0};

  if (read_http_request(client_fd, &req_raw) == HTTP_PARSE_INVALID) {
    debug_log("Failed to read HTTP request");
    close(client_fd);
    return;
  }

  if (parse_http_request(&req_raw, &req) == HTTP_PARSE_INVALID) {
    debug_log("Failed to parse HTTP request");
    close(client_fd);
    return;
  }

  char sanitized_path[buffer_size];
  sanitize_path(req.path, sanitized_path, buffer_size);

  init_http_response(&res);

  if (!handle_request(&req, &res))
    serve_file(sanitized_path, &res);

  send_http_response(client_fd, &res);

  debug_log("Response sent and client connection closed");
  free_http_headers(&req);
  free_http_response(&res);
  close(client_fd);
}

int main() {
  tcp_server server = {0};

  server_config config = {
      .port = 8080,
  };

  if (loadConfig(&config) == 0) {
    debug_log("failed to load config, using default values.");
  }

  server_status_e status = bind_tcp_port(&server, config.port);
  if (status != SERVER_OK) {
    debug_log("Server initialization failed");
    exit(EXIT_FAILURE);
  }

  size_t no_of_routes;
  no_of_routes = install_routes();
  no_of_routes > 0 ? debug_log("Routes installed")
                   : debug_log("No routes installed");

  for (;;) {
    int client_fd = accept_client(server.socket_fd);
    if (client_fd == -1) {
      debug_log("Failed to accept client connection");
      close(server.socket_fd);
      exit(EXIT_FAILURE);
    }

    debug_log("Client connected");

    handle_client(client_fd);
  }

  close(server.socket_fd);
  return 0;
}

/*
$ ./bin/httpserver
Server bound and listening on port 8080
DEBUG: Routes installed
DEBUG: Client connected
DEBUG: Response sent and client connection closed
DEBUG: Client connected
DEBUG: Response sent and client connection closed

$ curl localhost:8080/hello
Hello World!
$ curl localhost:8080/
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Welcome</title>
</head>
<body>
    <h1>Welcome to the Toy HTTP Server!</h1>
    <p>This is the index page.</p>
</body>
</html>
*/

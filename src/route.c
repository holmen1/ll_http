#include "http.h"
#include "route.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Route routes[MAX_ROUTES];
int route_count = 0;

void hello_controller(http_request *req, http_response *res);
void reverse_controller(http_request *req, http_response *res);
static void reverse(char *str, size_t len);

static void install_route(http_method_e method, const char *path,
                     void (*controller)(http_request *req, http_response *res)) {
  if (route_count < MAX_ROUTES) {
    routes[route_count].method = method;
    strcpy(routes[route_count].path, path);
    routes[route_count].controller = controller;
    ++route_count;
  }
}

size_t install_routes() {
  install_route(GET, "/hello", hello_controller);
  install_route(POST, "/reverse", reverse_controller);
  return route_count;
}

void hello_controller(http_request *req, http_response *res) {
  (void)req; // Unused in this handler
  res->status_code = 200;

  if (!res->body)
    res->body = malloc(64);

  strcpy(res->body, "Hello World!\n");
  res->body_length = 13;
}

void reverse_controller(http_request *req, http_response *res) {
  size_t len;
  if (!res->body)
    res->body = malloc(64);

  if ((len = strlen(req->body)) == 0) {
    res->status_code = 204;
    strcpy(res->body, "Nothing to reverse\n");
    res->body_length = strlen(res->body);
    return;
  }

  res->status_code = 200;
  char *tmp;
  tmp = malloc(len);
  strcpy(tmp, req->body);
  reverse(tmp, len);

  add_http_header(res, "Content-Type", "text/plain");
  set_http_body(res, tmp);

  free(tmp);
}

static void reverse(char *str, size_t len) {
  int c;
  for (int i = 0, j = len - 1; i < j; i++, j--) {
       c = str[i];
       str[i] = str[j];
       str[j] = c;
  }
}

#ifndef ROUTE_H
#define ROUTE_H

#include "http.h"

#define MAX_ROUTES 100

typedef struct {
  http_method_e method;
  char path[128];
  void (*controller)(http_request *, http_response *);
} Route;

size_t install_routes();

#endif

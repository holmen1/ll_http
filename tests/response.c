#include "http.h"
#include <stdio.h>
#include <string.h>

int main() {
    http_response response;
    init_http_response(&response);

    add_http_header(&response, "Content-Type", "text/html");
    add_http_header(&response, "Connection", "close");
    set_http_body(&response, "hello");

    size_t response_length;
    char *resp_string = construct_http_response(&response, &response_length);

    printf("\n---RESPONSE START---\n");
    fwrite(resp_string, 1, response_length, stdout);
    printf("\n---RESPONSE END---\n");

    free_http_response(&response);

    return 0;
}

/*
$ gcc -I../include -o test_response response.c ../obj/http.o
$ ./test_response 

---RESPONSE START---
HTTP/1.1 200 OK          <- Status line
Content-Type: text/html  <- Headers
Connection: close        <- Headers
                         <- Empty line (required HTTP delimiter)
hello                    <- Body content
---RESPONSE END---*/
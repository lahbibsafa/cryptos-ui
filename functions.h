#ifndef INTERFACE_CRYPTOS_CLOUD_FUNCTIONS_H
#define INTERFACE_CRYPTOS_CLOUD_FUNCTIONS_H
#include "global.h"
#include <jansson.h>
#include <ulfius.h>
json_t* response_to_json(struct _u_response *response);
int login(char* username, char* password);
void signup(char* username , char* password);
void downloadFile(char* fileName);
#endif //INTERFACE_CRYPTOS_CLOUD_FUNCTIONS_H

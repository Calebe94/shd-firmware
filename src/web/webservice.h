#ifndef _WEBSERVICE_H_
#define _WEBSERVICE_H_

#define WEBSERVICE_DOMAIN      "hidrometro"

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void webservice_init();

void notFound(AsyncWebServerRequest *request);

#endif

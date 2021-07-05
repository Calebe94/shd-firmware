set(srcs "tiny_webservice.c")

idf_component_register(SRCS "${srcs}"   INCLUDE_DIRS "." REQUIRES esp_http_server json mdns)

set(COMPONENT_NAME "tiny_webservice")

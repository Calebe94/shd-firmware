set(srcs "tiny_wifi.c")

set(COMPONENT_NAME "tiny_wifi")

idf_component_register(
    SRCS "${srcs}"   
    INCLUDE_DIRS "."
    REQUIRES 
        fatfs 
        spiffs 
        nvs_flash
)

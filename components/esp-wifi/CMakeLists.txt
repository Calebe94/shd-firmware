set(srcs "esp-wifi.c")

set(COMPONENT_NAME "esp-wifi")

idf_component_register(
    SRCS "${srcs}"   
    INCLUDE_DIRS "."
    REQUIRES 
        fatfs 
        spiffs 
        nvs_flash
)
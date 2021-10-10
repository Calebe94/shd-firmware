set(
    srcs 
        "flash_storage.cpp"
        "sdcard.cpp"
)

idf_component_register(
    SRCS "${srcs}" 
    INCLUDE_DIRS "." 
    REQUIRES 
        fatfs 
        spiffs 
        nvs_flash
        sdmmc
)

set(COMPONENT_NAME "tiny_storage")

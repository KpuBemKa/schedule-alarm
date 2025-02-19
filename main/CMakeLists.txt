idf_component_register(SRCS "mount.cpp" "main.cpp"
                           
                    INCLUDE_DIRS "."
                #     EMBED_FILES "favicon.ico"
                #                 "upload_script.html"
                    PRIV_REQUIRES log
                                  freertos
                                  esp_wifi
                                  nvs_flash
                                  spiffs
                                  fatfs
                                  esp_driver_gpio
                                  
                                  http_controller
                                  wifi_controller
                                  sleep
                                  schedule
                                  settings
                                  util
)

spiffs_create_partition_image(storage ../data_image FLASH_IN_PROJECT)
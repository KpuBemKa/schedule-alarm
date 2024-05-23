idf_component_register(SRCS "main.cpp"
                            "mount.cpp"
                    INCLUDE_DIRS "."
                #     EMBED_FILES "favicon.ico"
                #                 "upload_script.html"
                    PRIV_REQUIRES util
                                  log
                                  freertos
                                  esp_wifi
                                  nvs_flash
                                  http_server
                                  spiffs
                                  fatfs
                                  wifi_manager
)

spiffs_create_partition_image(storage ../data_image FLASH_IN_PROJECT)
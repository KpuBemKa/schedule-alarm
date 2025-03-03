idf_component_register(SRCS "main.cpp"                           
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES log
                                  freertos
                                  esp_wifi
                                  nvs_flash
                                  spiffs
                                  fatfs
                                  esp_driver_gpio
                                  esp_driver_gptimer
                                  
                                  http_controller
                                  wifi_controller
                                  sleep
                                  schedule
                                  settings
                                  util
)

spiffs_create_partition_image(storage ../data_image FLASH_IN_PROJECT)
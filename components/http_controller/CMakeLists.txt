idf_component_register(SRCS "http_controller.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES esp_http_server
                       PRIV_REQUIRES freertos
                                     json
                                     util
                                     settings)


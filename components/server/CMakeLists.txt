idf_component_register(SRCS "http_server.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES esp_http_server
                       PRIV_REQUIRES freertos
                                     json
                                     utility
                                     settings)


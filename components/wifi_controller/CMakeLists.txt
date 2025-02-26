idf_component_register(SRCS "sntp.cpp" "wifi_controller.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi
                       PRIV_REQUIRES settings
                                     log
)
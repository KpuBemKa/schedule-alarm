idf_component_register(SRCS "sleep.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       PRIV_REQUIRES freertos
                                     http_controller
                                     wifi_controller
                                     esp_hw_support
                                     settings
                                     esp_driver_gpio
                                     schedule
)


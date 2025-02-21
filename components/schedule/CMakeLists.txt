idf_component_register(SRCS "schedule.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES freertos
                       PRIV_REQUIRES settings
                                     json
)


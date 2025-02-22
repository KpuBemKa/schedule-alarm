idf_component_register(SRCS "settings.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES hal
                       PRIV_REQUIRES json
                                     log
                                     util
)


idf_component_register(SRCS "uri_encoder/uri_encoder.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES json
                       PRIV_REQUIRES log
)
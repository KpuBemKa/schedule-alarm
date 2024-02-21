idf_component_register(SRCS "ImpulseRelay.cpp"
                            "ConstantRelay.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       PRIV_REQUIRES driver
                                     )


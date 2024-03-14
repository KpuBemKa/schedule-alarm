idf_component_register(SRCS "models/schedule/ScheduleModel.cpp"
                            "views/relay/RelayView.cpp"
                            "controllers/time/TimeController.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES relay
                                data_structs
                       PRIV_REQUIRES freertos)


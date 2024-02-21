idf_component_register(SRCS "models/schedule/ScheduleModel.cpp"
                            "views/relay/RelayView.cpp"
                            "views/relay/TimeController.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES relay
                                data_structs
                       PRIV_REQUIRES freertos)


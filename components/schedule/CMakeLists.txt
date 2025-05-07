idf_component_register(SRCS "schedule.cpp"
                            "schedule_types/schedule_daily.cpp"
                            "schedule_types/schedule_weekly.cpp"
                            "schedule_types/schedule_monthly.cpp"
                            "schedule_types/schedule_yearly.cpp"
                            "schedule_types/schedule_custom.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES freertos
                       PRIV_REQUIRES settings
                                     json
                                     util
)


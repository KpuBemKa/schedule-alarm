idf_component_register(SRCS "schedule.cpp"
                            "schedules/schedule_daily.cpp"
                            "schedules/schedule_monthly.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES freertos
                       PRIV_REQUIRES settings
                                     json
)


idf_component_register(SRCS "pollperiod.c"
                            "txpower.c"
                            "uuid.c"
                            "hexdump.c"
                            "sleep.c"
                            "./time/time_api.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "openthread" "coap_workload" "esp_timer")

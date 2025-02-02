idf_component_register(SRCS "./server/server.c"
                            "./server/print_packet.c"
                            "./server/observe.c"
                            "./client/request.c"
                            "./client/response_callback.c"
                            "./common/common.c"
                            "./common/coap_common.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "openthread" "utilities")

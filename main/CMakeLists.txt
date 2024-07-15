set(delay_server, CONFIG_EXPERIMENT EQUAL 5)
set(delay_client, CONFIG_EXPERIMENT EQUAL 6)

if(delay_client OR delay_server)

  message("---------------------------")
  message("")
  message("Building for Delay Client and Server")
  message("")
  message("---------------------------")

  idf_component_register(SRC_DIRS "src" "src/delay_server" "src/delay_client"
                         INCLUDE_DIRS "include")

else()

  message("---------------------------")
  message("")
  message("Building for Throughput and Packet Loss")
  message("")
  message("---------------------------")

  idf_component_register(SRC_DIRS "src"
                         INCLUDE_DIRS "include")

endif()
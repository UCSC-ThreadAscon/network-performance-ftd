macro(start_delimiter)

    message("---------------------------------------------------------------------------------")
    message("")

endmacro(start_delimiter)

macro(end_delimiter)

    message("")
    message("---------------------------------------------------------------------------------")

endmacro(end_delimiter)

if((CONFIG_EXPERIMENT EQUAL 3) OR (CONFIG_EXPERIMENT EQUAL 4))

  start_delimiter()
  message("Building for the Delay Client and Server.")
  end_delimiter()

  idf_component_register(SRC_DIRS "src" "src/delay_server" "src/delay_client"
                         INCLUDE_DIRS "include")

else()

  start_delimiter()
  message("Building for the Throughput and Packet Loss experiments.")
  end_delimiter()

  idf_component_register(SRC_DIRS "src"
                         INCLUDE_DIRS "include")

endif()
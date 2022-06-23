if(MODULE_jsoncpp_FOUND)
  # test program
  define_module(BINARY pysupport-test-json
    DEPENDS
    pysupport pydbglog
    )

  set(pysupport-test-json_SOURCES
    json.cpp
    )

  add_executable(pysupport-test-json ${pysupport-test-json_SOURCES})
  target_link_libraries(pysupport-test-json ${MODULE_LIBRARIES})
  target_compile_definitions(pysupport-test-json PRIVATE ${MODULE_DEFINITIONS})
  buildsys_binary(pysupport-test-json)
endif()

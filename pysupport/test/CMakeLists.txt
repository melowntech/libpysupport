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
buildsys_target_compile_definitions(pysupport-test-json ${MODULE_DEFINITIONS})
buildsys_binary(pysupport-test-json)

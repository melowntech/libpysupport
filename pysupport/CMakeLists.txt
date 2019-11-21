# bump version here
set(pysupport_VERSION 1.2)

set(pysupport_jsoncpp_SOURCES)
set(pysupport_DEPENDS)

if(MODULE_jsoncpp_FOUND)
  message(STATUS "pysupport: compiling in jsoncpp support")

  list(APPEND pysupport_DEPENDS jsoncpp)

  set(pysupport_jsoncpp_SOURCES json.hpp json.cpp)
else()
  message(STATUS "pysupport: compiling without jsoncpp support")
endif()

define_module(LIBRARY pysupport=${pysupport_VERSION}
  DEPENDS ${pysupport_DEPENDS} utility Boost_IOSTREAMS Boost_PYTHON PYTHONLIBS)

buildsys_compile_with_install_prefix()

file_to_cpp(pysupport_PY_SOURCES pysupport::detail::import import.py)
file_to_cpp(pysupport_PY_SOURCES pysupport::detail::load load.py)
py_to_pyc(pysupport_PY_SOURCES pysupport::detail::formatexception formatexception.py)

set(pysupport_SOURCES
  hasattr.hpp
  setattr.hpp
  class.hpp
  enum.hpp
  converters.hpp
  iostreams.hpp iostreams.cpp
  systemexit.hpp systemexit.cpp
  repr.hpp repr.cpp

  buffer.hpp buffer.cpp
  import.hpp import.cpp
  load.hpp load.cpp
  runnable.hpp runnable.cpp
  formatexception.hpp formatexception.cpp
  package.hpp package.cpp
  argv.hpp argv.cpp
  threads.hpp
  gc.hpp
  variant.hpp
  initialize.hpp initialize.cpp

  ${pysupport_jsoncpp_SOURCES}
  ${pysupport_PY_SOURCES}
)

add_library(pysupport ${pysupport_SOURCES})
target_link_libraries(pysupport ${MODULE_LIBRARIES})
buildsys_binary(pysupport)

add_subdirectory(test EXCLUDE_FROM_ALL)

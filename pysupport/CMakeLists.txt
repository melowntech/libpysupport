# bump version here
set(pysupport_VERSION 1.0)

define_module(LIBRARY pysupport=${pysupport_VERSION}
  DEPENDS utility Boost_IOSTREAMS Boost_PYTHON)

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
  ${pysupport_PY_SOURCES}
)

add_library(pysupport ${pysupport_SOURCES})
target_link_libraries(pysupport ${MODULE_LIBRARIES})
buildsys_binary(pysupport)

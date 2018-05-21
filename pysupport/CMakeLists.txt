# bump version here
set(pysupport_VERSION 1.0)

define_module(LIBRARY pysupport=${pysupport_VERSION}
  DEPENDS utility Boost_PYTHON)

buildsys_compile_with_install_prefix()

file_to_cpp(pysupport_PY_SOURCES pysupport::detail::import import.py)
file_to_cpp(pysupport_PY_SOURCES pysupport::detail::load load.py)
py_to_pyc(pysupport_PY_SOURCES pysupport::detail::formatexception formatexception.py)

set(pysupport_SOURCES
  hasattr.hpp
  buffer.hpp buffer.cpp
  import.hpp import.cpp
  load.hpp load.cpp
  runnable.hpp runnable.cpp
  formatexception.hpp formatexception.cpp
  package.hpp package.cpp
  ${pysupport_PY_SOURCES}
)

add_library(pysupport ${pysupport_SOURCES})
target_link_libraries(pysupport ${MODULE_LIBRARIES})
buildsys_binary(pysupport)

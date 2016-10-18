define_module(LIBRARY pysupport DEPENDS utility Boost_PYTHON)

buildsys_compile_with_install_prefix()

file_to_cpp(pysupport_PY_SOURCES pysupport::detail::import import.py)
file_to_cpp(pysupport_PY_SOURCES pysupport::detail::load load.py)
py_to_pyc(pysupport_PY_SOURCES pysupport::detail::formatexception formatexception.py)

set(pysupport_SOURCES
  buffer.hpp buffer.cpp
  import.hpp import.cpp
  load.hpp load.cpp
  runnable.hpp runnable.cpp
  formatexception.hpp formatexception.cpp
  ${pysupport_PY_SOURCES}
)

add_library(pysupport ${pysupport_SOURCES})
target_link_libraries(pysupport ${MODULE_LIBRARIES})
buildsys_binary(pysupport)

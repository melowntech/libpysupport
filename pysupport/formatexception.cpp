#include <string.h>

#include <thread>

#include "dbglog/dbglog.hpp"

#include "./string.hpp"
#include "./load.hpp"
#include "./formatexception.hpp"

#include "formatexception.py.pyc.hpp"

namespace pysupport {

namespace detail {

std::once_flag onceFlag;

boost::python::object formatter;

} // namespace detail

std::string formatCurrentException()
{
    namespace py = boost::python;
    try {
        // fetch exception
        ::PyObject *rawType(nullptr);
        ::PyObject *rawValue(nullptr);
        ::PyObject *rawTraceback(nullptr);
        ::PyErr_Fetch(&rawType, &rawValue, &rawTraceback);

        typedef py::handle< ::PyObject> Handle;

        Handle type(py::allow_null(rawType));
        Handle value(py::allow_null(rawValue));
        Handle traceback(py::allow_null(rawTraceback));

        std::call_once(detail::onceFlag, [&]()
        {
            detail::formatter
                = load("formatexception"
                       , detail::formatexception).attr("format");
        });

        return py2utf8(detail::formatter(type, value, traceback));
    } catch (const boost::python::error_already_set&) {
        ::PyErr_Print();
    }
    return "Failed to get python exception.";
}

} // namespace pysupport

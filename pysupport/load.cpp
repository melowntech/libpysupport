#include <string.h>

#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>

#include "dbglog/dbglog.hpp"

#include "load.hpp"
#include "buffer.hpp"

#include "load.py.hpp"

namespace pysupport {

boost::python::object load(const std::string &name
                           , const void *data, std::size_t size)
{
    using namespace boost::python;
    try {
        // TODO: use Py_CompileStringFlags and PyImport_ExecCodeModuleEx
        // get globals
        object globals = boost::python::import("__main__").attr("__dict__");

        dict locals;
        locals["fullname"] = name;
        locals["contents"] = buffer(data, size);
        locals["compiled"] = true;

        str code(reinterpret_cast<const char*>(detail::load)
                 , sizeof(detail::load));
        exec(code, globals, locals);
        return locals["module"];
    } catch (const error_already_set&) {
        ::PyErr_Print();
        handle_exception();
        throw;
    }
}

} // namespace pysupport

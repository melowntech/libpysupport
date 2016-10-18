#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>

#include "dbglog/dbglog.hpp"

#include "import.hpp"

#include "import.py.hpp"

namespace fs = boost::filesystem;

namespace pysupport {

boost::python::object import(const fs::path &path)
{
    using namespace boost::python;

    // get globals
    object globals = boost::python::import("__main__").attr("__dict__");

    dict locals;
    locals["name"] = path.stem().string();
    locals["path"] = path.parent_path().string();

    str src(reinterpret_cast<const char*>(detail::import)
            , sizeof(detail::import));

    exec(src, globals, locals);
    return locals["module"];
}

} // namespace pysupport

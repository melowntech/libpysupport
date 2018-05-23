/**
 * Copyright (c) 2018 Melown Technologies SE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>
#include <mutex>

#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/python/class.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "dbglog/dbglog.hpp"

#include "./package.hpp"
#include "./converters.hpp"

namespace bp = boost::python;
namespace fs = boost::filesystem;

namespace pysupport {

const char* packageName("melown");

struct to_python_path {
    static PyObject* convert(const fs::path &obj) {
        return bp::incref(bp::object(obj.string()).ptr());
    }
};

/** Creates from-python converter for boost::filesystem::path
 *  None is mapped to boost::none
 */
struct from_python_path
{
    typedef boost::python::converter::rvalue_from_python_stage1_data Data;
    typedef boost::python::converter::rvalue_from_python_storage<
        fs::path> Storage;

    from_python_path() {
        boost::python::converter::registry::push_back
            (&convertible, &construct, boost::python::type_id<fs::path>());
    }

    // Determine if ptr can be converted in a Optional
    static void* convertible(::PyObject *ptr) {
        return PyUnicode_Check(ptr) ? ptr : nullptr;
    }

    // Convert ptr into a fs::path
    static void construct(::PyObject *ptr, Data *data) {
        // get memory
        void *storage(((Storage*) data)->storage.bytes);

        new (storage) fs::path(boost::python::extract<std::string>(ptr));

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

} // namespace pysupport

BOOST_PYTHON_MODULE(melown)
{
    using namespace bp;

    to_python_converter<fs::path, pysupport::to_python_path>();
    pysupport::from_python_path();

    PYSUPPORT_OPTIONAL(std::string);
    PYSUPPORT_OPTIONAL(fs::path);

    class_<std::vector<double>>("double_list")
        .def(vector_indexing_suite<std::vector<double>>())
        ;
}

namespace pysupport {

namespace {
std::once_flag onceFlag;
} // namespace

bp::object package()
{
    typedef bp::handle< ::PyObject> Handle;

    std::call_once(onceFlag, [&]()
    {
#if PY_MAJOR_VERSION == 2
        initmelown();
#else
        Handle module(::PyInit_melown());
        auto sys(bp::import("sys"));
        sys.attr("modules")[pysupport::packageName] = module;
#endif
    });

    return bp::import(packageName);
}

void addModuleToPackage(const char *name, ::PyObject *module)
{
    auto p(package());

    typedef bp::handle< ::PyObject> Handle;
    Handle m(module);

    if (-1 == ::PyModule_AddObject(p.ptr(), name, bp::incref(module))) {
        LOGTHROW(err2, std::runtime_error)
            << "PyModule_AddObject failed";
    }

    auto sys(bp::import("sys"));
    sys.attr("modules")[str(boost::format("%s.%s")
                            % packageName % name)] = m;
}

} // namespace pysupport

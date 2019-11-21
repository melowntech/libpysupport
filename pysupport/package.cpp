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

#include "package.hpp"
#include "converters.hpp"
#include "iostreams.hpp"
#include "array.hpp"

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

    // Determine if ptr can be converted in an Optional
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
    pysupport::registerIOStreams();

    PYSUPPORT_OPTIONAL(std::string);
    PYSUPPORT_OPTIONAL(fs::path);
    PYSUPPORT_OPTIONAL(signed int);
    PYSUPPORT_OPTIONAL(signed short);
    PYSUPPORT_OPTIONAL(signed long);
    PYSUPPORT_OPTIONAL(unsigned int);
    PYSUPPORT_OPTIONAL(unsigned short);
    PYSUPPORT_OPTIONAL(unsigned long);
    PYSUPPORT_OPTIONAL(float);
    PYSUPPORT_OPTIONAL(double);

    class_<std::vector<double>>("double_list")
        .def(vector_indexing_suite<std::vector<double>>())
        ;

    class_<std::vector<std::string>>("string_list>")
        .def(vector_indexing_suite<std::vector<std::string>>())
        ;

#define PYSUPPORT_REGISTER_STD_ARRAY(T, N)                          \
    class_<std::array<T, N>>(#T #N  "_array")                      \
        .def(pysupport::array_indexing_suite<std::array<T, N>>())

    PYSUPPORT_REGISTER_STD_ARRAY(double, 2);
    PYSUPPORT_REGISTER_STD_ARRAY(double, 3);
    PYSUPPORT_REGISTER_STD_ARRAY(double, 4);
    PYSUPPORT_REGISTER_STD_ARRAY(double, 5);

#undef PYSUPPORT_REGISTER_STD_ARRAY
}

namespace pysupport {

namespace {
std::once_flag onceFlag;
} // namespace

bp::object package()
{
    std::call_once(onceFlag, [&]()
    {
#if PY_MAJOR_VERSION == 2
        initmelown();
#else
        bp::handle<> module(::PyInit_melown());
        auto sys(bp::import("sys"));
        sys.attr("modules")[pysupport::packageName] = module;
#endif
    });

    return bp::import(packageName);
}

void addModuleToPackage(const char *name, ::PyObject *module
                        , const PackageCallback &callback)
{
    auto p(package());

    bp::handle<> m(module);

    if (-1 == ::PyModule_AddObject(p.ptr(), name, bp::incref(module))) {
        LOG(warn2) << "Failed to add module <" << name
                   << "> to package <melown>.";
        bp::throw_error_already_set();
    }

    auto sys(bp::import("sys"));
    sys.attr("modules")[str(boost::format("%s.%s")
                            % packageName % name)] = m;

    if (callback) { callback(p, bp::object(m)); }
}

void addModuleToPackage(const char *name, ::PyObject *module
                        , const char *packageName, const bp::object &package
                        , const PackageCallback &callback)
{
    bp::handle<> m(module);

    if (-1 == ::PyModule_AddObject(package.ptr(), name, bp::incref(module))) {
        LOG(warn2) << "Failed to add module <" << name
                   << "> to package <" << packageName << ">.";
        bp::throw_error_already_set();
    }

    auto sys(bp::import("sys"));
    sys.attr("modules")[str(boost::format("%s.%s")
                            % packageName % name)] = m;

    if (callback) { callback(package, bp::object(m)); }
}

} // namespace pysupport

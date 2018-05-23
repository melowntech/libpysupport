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

#include <mutex>

#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

#include "dbglog/dbglog.hpp"

#include "./package.hpp"
#include "./converters.hpp"

namespace bp = boost::python;

namespace pysupport {

const char* packageName("melown");

struct to_python_path {
    static PyObject* convert(const boost::filesystem::path &obj) {
        return bp::incref(bp::object(obj.string()).ptr());
    }
};

} // namespace pysupport

BOOST_PYTHON_MODULE(melown)
{
    bp::to_python_converter<
        boost::filesystem::path, pysupport::to_python_path>();

    bp::to_python_converter<
        boost::optional<std::string>
        , pysupport::to_python_optional<std::string>>();

    bp::to_python_converter<
        boost::optional<boost::filesystem::path>
        , pysupport::to_python_optional<boost::filesystem::path>>();
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

/**
 * Copyright (c) 2017 Melown Technologies SE
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

#include <pysupport/boost-python-definitions.hpp>

#include <Python.h>

#include <boost/filesystem.hpp>

#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <boost/python/stl_iterator.hpp>

#include "dbglog/dbglog.hpp"

#include "utility/zip.hpp"

#include "import.hpp"

#include "import.py.hpp"

namespace fs = boost::filesystem;

namespace pysupport {

namespace {

enum ModuleType { other, zip, dir };

ModuleType moduleType(const fs::path &path)
{
    if (fs::is_directory(path)) { return ModuleType::dir; }
    if (utility::zip::Reader::check(path)) { return ModuleType::zip; }
    return ModuleType::other;
}

boost::python::object helper(ModuleType type, const std::string &path
                             , const std::string &name
                             , const char *useName
                             , const boost::python::object &wrapper)
{
    using namespace boost::python;

    object globals(boost::python::import("__main__").attr("__dict__"));
    // auto bi(boost::python::import("builtins"));
    // object m(handle<>(PyModule_New("__importer")));
    // PyModule_AddObject(m.ptr(), "__builtins__", bi.ptr());
    // Py_INCREF(bi.ptr());
    // auto globals(m.attr("__dict__"));

    dict locals;
    switch (type) {
    case ModuleType::zip: locals["module_type"] = "zip"; break;
    case ModuleType::dir: locals["module_type"] = "dir"; break;
    case ModuleType::other: locals["module_type"] = object(); break;
    }

    locals["name"] = name;
    locals["useName"] = (useName ? useName : name.c_str());
    locals["path"] = path;
    locals["wrapper"] = wrapper;

    {
        std::string code(reinterpret_cast<const char*>(detail::import)
                         , sizeof(detail::import));
        auto result(::PyRun_StringFlags(code.c_str(), Py_file_input
                                        , globals.ptr(), locals.ptr()
                                        , nullptr));
        if (!result) { throw_error_already_set(); }
        Py_DECREF(result);
    }

    return locals["module"];
}

} // namespace

boost::python::object import(const fs::path &path, const char *useName
                             , const boost::python::object &wrapper)
{
    const auto type(moduleType(path));

    if (type != ModuleType::other) {
        return helper(type, path.string(), "__main__", useName, wrapper);
    }
    return helper(type, path.parent_path().string()
                  , path.stem().string(), useName, wrapper);
}

} // namespace pysupport

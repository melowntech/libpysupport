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

#ifndef pysupport_package_hpp_included_
#define pysupport_package_hpp_included_

#include <mutex>

#include "boost-python-definitions.hpp"
#include <boost/python.hpp>

namespace pysupport {

/** Creates 'melown' package (empty module) and injects it into sys["modules"].
 */
boost::python::object package();

typedef std::function<void(const boost::python::object&
                           , const boost::python::object&)>
PackageCallback;

/** Adds given module to 'melown' package (which is created for the first time).
 */
void addModuleToPackage(const char *name, ::PyObject *module
                        , const PackageCallback &callback = PackageCallback());

/** Adds given module to given existing package
 */
void addModuleToPackage(const char *name, ::PyObject *module
                        , const boost::python::object *package = nullptr
                        , const PackageCallback &callback = PackageCallback());

/** Adds given module to provided package.
 */
void addModuleToPackage(const char *name, ::PyObject *module
                        , const char *packageName
                        , const boost::python::object &package
                        , const PackageCallback &callback = PackageCallback());

#if PYSUPPORT_MODULE_IMPORT_API >= 2
#  define PYSUPPORT_MODULE_IMPORT_CALLBACK(name, cb)                \
    namespace { std::once_flag name##_onceFlag; }                   \
                                                                    \
    boost::python::object                                           \
    import(const boost::python::object *package)                    \
    {                                                               \
        std::call_once(name##_onceFlag, [&]()                       \
        {                                                           \
            pysupport::addModuleToPackage                           \
                (#name, PyInit_melown_##name(), package, cb);       \
        });                                                         \
                                                                    \
        return bp::import("sys").attr("modules")["melown."#name];   \
    }
#else
#  define PYSUPPORT_MODULE_IMPORT_CALLBACK(name, cb)                \
    namespace { std::once_flag name##_onceFlag; }                   \
                                                                    \
    boost::python::object import() {                                \
        std::call_once(name##_onceFlag, [&]()                       \
        {                                                           \
            pysupport::addModuleToPackage                           \
                (#name, PyInit_melown_##name(), cb);                \
        });                                                         \
                                                                    \
        return bp::import("melown."#name);                          \
    }
#endif

#define PYSUPPORT_MODULE_IMPORT(name)                                   \
    PYSUPPORT_MODULE_IMPORT_CALLBACK(name, pysupport::PackageCallback())

} // namespace pysupport

#endif // pysupport_package_hpp_included_

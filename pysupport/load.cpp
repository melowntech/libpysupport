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

#include <string.h>

#include <pysupport/boost-python-definitions.hpp>

#include <Python.h>

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
    // get globals
    object globals(boost::python::import("__main__").attr("__dict__"));

    dict locals;
    locals["fullname"] = name;
    locals["contents"] = buffer(data, size);
    locals["compiled"] = true;

    {
        std::string code(reinterpret_cast<const char*>(detail::load)
                         , sizeof(detail::load));

        auto result(::PyRun_StringFlags(code.c_str(), Py_file_input
                                        , globals.ptr(), locals.ptr()
                                        , nullptr));
        if (!result) { throw_error_already_set(); }
        Py_DECREF(result);
    }
    return locals["module"];
}

} // namespace pysupport

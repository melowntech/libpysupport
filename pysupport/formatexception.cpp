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

#include <thread>
#include <mutex>

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
        ::PyErr_NormalizeException(&rawType, &rawValue, &rawTraceback);

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

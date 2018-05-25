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

#include <boost/python.hpp>

#include "./systemexit.hpp"

namespace bp = boost::python;

namespace pysupport {

boost::optional<int> getSystemExit()
{
    if (::PyErr_ExceptionMatches(::PyExc_SystemExit)) {
        // system exit -> get exception and extract code
        ::PyObject *rawType(nullptr);
        ::PyObject *rawValue(nullptr);
        ::PyObject *rawTraceback(nullptr);
        ::PyErr_Fetch(&rawType, &rawValue, &rawTraceback);
        ::PyErr_NormalizeException(&rawType, &rawValue, &rawTraceback);

        if (!rawValue) { return EXIT_FAILURE; }

        bp::handle<> type(rawType);
        bp::handle<> value(bp::allow_null(rawValue));
        bp::handle<> traceback(bp::allow_null(rawTraceback));

        return boost::optional<int>
            (bp::extract<int>(bp::object(value).attr("code")));
    }
    return boost::none;
}

} // namespace pysupport

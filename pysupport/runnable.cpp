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

#include <boost/python.hpp>

#include "runnable.hpp"

namespace pysupport {

namespace {
    const char *RUNNABLE_NAME("runnable");
} // namespace

class RunnableWrapper {
public:
    RunnableWrapper(boost::python::object value)
        : value_()
    {
        auto v(::PyCapsule_GetPointer(value.ptr(), RUNNABLE_NAME));
        if (!v) {
            boost::python::throw_error_already_set();
        }
        value_ = static_cast<utility::Runnable *>(v);
    }

    void operator()() {
        if (!value_ || !*value_) {
            PyErr_SetString(PyExc_KeyboardInterrupt
                            , "Process not running.");
            boost::python::throw_error_already_set();
        }
    }

private:
    utility::Runnable *value_;
};

/** Runnables module from given path.
 */
boost::python::object runnable(utility::Runnable &runnable)
{
    using namespace boost::python;
    object RunnableClass = class_<RunnableWrapper>
        ("Runnable", init<object>())
        .def("__call__", &RunnableWrapper::operator())
        ;

    auto pycaps(::PyCapsule_New(&runnable, RUNNABLE_NAME, nullptr));
    if (!pycaps) {
        throw_error_already_set();
    }
    object r = RunnableClass(object(handle<>(borrowed<>(pycaps))));

    return r;
}

} // namespace pysupport

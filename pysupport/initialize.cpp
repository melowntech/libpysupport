/**
 * Copyright (c) 2019 Melown Technologies SE
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

#include "initialize.hpp"

namespace bp = boost::python;

namespace pysupport {

namespace {

bp::object sys_module;
bp::object atexit_module;

void cleanup()
{
    if (atexit_module) {
        // try to run atexit handlers since we do not finalize
        try {
            atexit_module.attr("_run_exitfuncs")();
        } catch (...) {}
    }

    if (sys_module) {
        // flush python streams
        try {
            sys_module.attr("stdout").attr("flush")();
            sys_module.attr("stderr").attr("flush")();
        } catch (...) {}
    }

    ::fflush(stdout);
    ::fflush(stderr);
}

} // namespace

void initialize(bool registerAtExit)
{
    // Initialize python interpreter
    ::Py_Initialize();

    if (!registerAtExit) { return; }


    // since we cannot call Py_Finalize (c'mon Boost.Python...) we need to
    // have our own at exit handler

    try {
        sys_module = bp::import("sys");
    } catch (...) {}

    try {
        atexit_module = bp::import("atexit");
    } catch (...) {}

    std::atexit(cleanup);
}

} // namespace pysupport

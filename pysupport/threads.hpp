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

#ifndef pysupport_threads_hpp_included_
#define pysupport_threads_hpp_included_

#include <pysupport/boost-python-definitions.hpp>

#include <Python.h>

namespace pysupport {

/** RAII-styled scoped guard to enable threads, i.e. temporarily unlock GIL.
 */
class EnableThreads {
public:
    EnableThreads() : state_(::PyEval_SaveThread()) {}
    ~EnableThreads() { ::PyEval_RestoreThread(state_); }

private:
    ::PyThreadState *state_;
};

/** RAII-styled scoped guard to temporarily request GIL.
 */
class EnablePython {
public:
    EnablePython() : state_(::PyGILState_Ensure()) {}
    ~EnablePython() { PyGILState_Release(state_); }

private:
    ::PyGILState_STATE state_;
};

} // namespace pysupport

#endif // pysupport_threads_hpp_included_


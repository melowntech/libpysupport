/**
 * Copyright (c) 2020 Melown Technologies SE
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

#ifndef pysupport_debugger_hpp_included_
#define pysupport_debugger_hpp_included_

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/python/object.hpp>

#include "utility/enum-io.hpp"

namespace pysupport {


struct DebuggerOptions {
    bool enable;

    DebuggerOptions()
        : enable(false)
    {}
};

class Debugger {
public:
    /** Dummy debugger.
     */
    Debugger() {}

    Debugger(const DebuggerOptions &options);

    Debugger& operator=(Debugger &&p) {
        if (this != &p) {
            options_ = p.options_;
            std::swap(module_, p.module_);
            std::swap(runcall_, p.runcall_);
        }
        return *this;
    }

    Debugger(const Debugger&) = delete;
    Debugger& operator=(const Debugger&) = delete;

    operator bool() const { return module_; }

    /** Calls runnable(args). Skips debugging if not configured
     */
    template <typename ...Args>
    boost::python::object operator()(const boost::python::object &runnable
                                     , Args &&...args)
    {
        if (runcall_) {
            return runcall_(runnable, std::forward<Args>(args)...);
        }
        return runnable(std::forward<Args>(args)...);
    }

private:
    DebuggerOptions options_;
    boost::python::object module_;
    boost::python::object runcall_;
};


} // namespace pysupport

#endif // pysupport_debugger_hpp_included_

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

#include <boost/lexical_cast.hpp>

#include <boost/python/import.hpp>

#include "dbglog/dbglog.hpp"

#include "string.hpp"
#include "profiler.hpp"

namespace bp = boost::python;
namespace fs = boost::filesystem;

namespace pysupport {

bp::object createProfiler(const ProfilerOptions &options)
{
    if (!options.module) { return {}; }

    auto module
        (bp::import
         (boost::lexical_cast<std::string>(*options.module).c_str()));

    auto Profile(module.attr("Profile"));

    // TODO: pass args
    return Profile();
}

Profiler::Profiler(const ProfilerOptions &options)
    : options_(options)
    , profiler_(createProfiler(options))
{
    if (profiler_) {
        runcall_ = profiler_.attr("runcall");
    }
}

void Profiler::stats()
{
    if (!profiler_) { return; }
    if (!(options_.printStats || options_.saveStats)) { return; }

    profiler_.attr("create_stats")();
    if (options_.printStats) {
        if (options_.printStatsSort) {
            profiler_.attr("print_stats")
                (boost::lexical_cast<std::string>(*options_.printStatsSort)
                 .c_str());
        } else {
            profiler_.attr("print_stats")();
        }
    }

    if (options_.saveStats) {
        profiler_.attr("dump_stats")(options_.saveStats->string());
    }
}

} // namespace pysupport

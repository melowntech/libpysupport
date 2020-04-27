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

#ifndef pysupport_profiler_hpp_included_
#define pysupport_profiler_hpp_included_

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/python/object.hpp>

#include "utility/enum-io.hpp"

namespace pysupport {


UTILITY_GENERATE_ENUM(ProfilerModule,
                      ((cProfile))
                      ((profile))
                      )

UTILITY_GENERATE_ENUM(ProfilerSort,
                      ((calls))
                      ((cumulative))
                      ((cumtime))
                      ((file))
                      ((filename))
                      ((module))
                      ((ncalls))
                      ((pcalls))
                      ((line))
                      ((name))
                      ((nfl))
                      ((stdname))
                      ((time))
                      ((tottime))
                      )

struct ProfilerOptions {
    boost::optional<ProfilerModule> module;

    bool builtins;
    double timeunit;

    bool printStats;
    boost::optional<ProfilerSort> printStatsSort;

    boost::optional<boost::filesystem::path> saveStats;

    bool hookMultiprocessing;

    ProfilerOptions()
        : builtins(true), timeunit(0.0)
        , printStats(true), hookMultiprocessing(true)
    {}

    operator bool() const { return bool(module); }
};

class Profiler {
public:
    /** Dummy profiler.
     */
    Profiler() {}

    Profiler(const ProfilerOptions &options);

    Profiler& operator=(Profiler &&p) {
        if (this != &p) {
            options_ = p.options_;
            std::swap(profiler_, p.profiler_);
            std::swap(runcall_, p.runcall_);
            std::swap(detail_, p.detail_);
        }
        return *this;
    }

    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    operator bool() const { return profiler_; }

    /** Calls runnable(args). Skips profiling if not configured
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

    boost::python::object runcall() const { return runcall_; }

    void stats();

    const ProfilerOptions& options() const { return options_; }

    struct Detail;

private:
    ProfilerOptions options_;
    boost::python::object profiler_;
    boost::python::object runcall_;

    std::shared_ptr<Detail> detail_;
};


} // namespace pysupport

#endif // pysupport_profiler_hpp_included_

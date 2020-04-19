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

#include <cstdio>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <boost/python/import.hpp>

#include "dbglog/dbglog.hpp"
#include "utility/path.hpp"

#include "string.hpp"
#include "setattr.hpp"
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

    bp::dict kwargs;
    kwargs["builtins"] = options.builtins;
    kwargs["timeunit"] = options.timeunit;

    return Profile(*bp::make_tuple(), **kwargs);
}

bp::object mp_Process;

struct Profiler::Detail : std::enable_shared_from_this<Profiler::Detail> {
public:
    typedef std::shared_ptr<Profiler::Detail> pointer;

    Detail(const ProfilerOptions &options, boost::python::object profiler)
        : options(options), profiler(profiler)
    {
    }

    static pointer init(const ProfilerOptions &options
                        , boost::python::object profiler);
    void stats(bool subprocess = false);

    const ProfilerOptions options;
    boost::python::object profiler;
};

void Process_run(const bp::object &self)
{
    LOG(info4) << "Before run";

    bp::object pyDetail(self.attr("_profiler_detail"));
    auto detail = bp::extract<Profiler::Detail::pointer>(pyDetail)();

    mp_Process.attr("_profiler_run")(self);
    LOG(info4) << "After run";
    detail->stats(true);
}

void initMultiprocessingHook(const Profiler::Detail::pointer &profiler)
{
    using namespace bp;

    if (!mp_Process) {
        auto mp(bp::import("multiprocessing"));
        mp_Process = mp.attr("Process");
    }

    setattr(mp_Process, "_profiler_run", mp_Process.attr("run"));

    auto Detail_cls = class_<Profiler::Detail, Profiler::Detail::pointer
                             , boost::noncopyable>
        ("Profiler_Detail", no_init)
        ;

    bp::setattr(mp_Process, "_profiler_detail", bp::object(profiler));
    bp::setattr(mp_Process, "run", bp::make_function(Process_run));
}

Profiler::Detail::pointer
Profiler::Detail::init(const ProfilerOptions &options
                       , boost::python::object profiler)
{
    auto detail(std::make_shared<Detail>(options, profiler));

    if (options.hookMultiprocessing) {
        initMultiprocessingHook(detail);
        LOG(info4) << "Process";
    }
    return detail;
}

void Profiler::Detail::stats(bool subprocess)
{
    if (!profiler) { return; }
    if (!(options.printStats || options.saveStats)) { return; }

    profiler.attr("create_stats")();
    if (options.printStats) {
        if (subprocess) {
            // cstdio because python uses the same
            std::printf("Profiling statistics of the sub-process %d and "
                        "all its parents:\n"
                        , dbglog::process_id());
        } else {
            std::printf("Profiling statistics of the main process:\n");
        }

        if (options.printStatsSort) {
            profiler.attr("print_stats")
                (boost::lexical_cast<std::string>(*options.printStatsSort)
                 .c_str());
        } else {
            profiler.attr("print_stats")();
        }
    }

    if (options.saveStats) {
        auto path(*options.saveStats);
        if (subprocess) {
            path = utility::addExtension
                (path, str(boost::format(".%s") % dbglog::process_id()));
        }
        LOG(info3) << "Saving profiling statistics in " << path << ".";
        profiler.attr("dump_stats")(path.string());
    }
}

Profiler::Profiler(const ProfilerOptions &options)
    : options_(options)
    , profiler_(createProfiler(options))
{
    if (!profiler_) { return; }
    runcall_ = profiler_.attr("runcall");
    detail_ = Detail::init(options_, profiler_);
}

void Profiler::stats()
{
    if (detail_) { detail_->stats(); }
}

} // namespace pysupport

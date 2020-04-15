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

#ifndef pysupport_profiler_po_hpp_included_
#define pysupport_profiler_po_hpp_included_

#include <boost/program_options.hpp>

#include "utility/streams.hpp"

#include "profiler.hpp"

namespace pysupport {

void configuration(ProfilerOptions &options
                   , boost::program_options::options_description &od)
{
    namespace po = boost::program_options;

    od.add_options()
        ("profile"
         , po::value<ProfilerModule>()
         ->implicit_value(ProfilerModule::cProfile)
         , utility::concat
         ("Profiler to use. Profiler is inactive if unset (default); one of "
          , enumerationString(ProfilerModule())
          , ".").c_str())

        ("profile.stats.print"
         , po::value(&options.printStats)->default_value(true)
         , "Print profiling statistics to output.")

        ("profile.stats.print.sort"
         , po::value<ProfilerSort>()
         , utility::concat
         ("Sorting order of printer profiling statistics; one of "
          , enumerationString(ProfilerSort())
          , ".").c_str())

        ("profile.stats.save"
         , po::value<boost::filesystem::path>()
         , "Write profiling statistics to given file.")
        ;
}

void configure(ProfilerOptions &options
               , const boost::program_options::variables_map &vars)
{
    namespace po = boost::program_options;

    if (!vars.count("profile")) { return; }

    options.module = vars["profile"].as<ProfilerModule>();

    if (vars.count("profile.stats.print.sort")) {
        options.printStatsSort = vars["profile.stats.print.sort"]
            .as<ProfilerSort>();
    }

    if (vars.count("profile.stats.save")) {
        options.saveStats
            = vars["profile.stats.save"].as<boost::filesystem::path>();
    }
}

} // namespace pysupport

#endif // pysupport_profiler_po_hpp_included_

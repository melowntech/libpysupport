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

#ifndef pysupport_argv_hpp_included_
#define pysupport_argv_hpp_included_

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/python.hpp>

#include <iostream>

namespace pysupport {

typedef std::vector<std::string> Argv;

/** Fill sys.argv with the content of provided vector.
 */
template <typename ...Args> void argv(Args &&...arg);

// inlines

namespace detail {

void argvImpl(const Argv &argv);

inline void addArg(Argv &argv, const std::string &arg)
{
    argv.push_back(arg);
}

inline void addArg(Argv &argv, const boost::filesystem::path &arg)
{
    argv.push_back(arg.string());
}

inline void addArg(Argv &argv, const Argv &arg)
{
    argv.insert(argv.end(), arg.begin(), arg.end());
}

template <typename T>
inline void addArg(Argv &argv, const boost::optional<T> &arg)
{
    if (arg) { addArg(argv, *arg); }
}

inline void buildArgv(Argv&) {}

template <typename T, typename ...Args>
void buildArgv(Argv &argv, T &&arg, Args &&...rest)
{
    addArg(argv, arg);
    buildArgv(argv, std::forward<Args>(rest)...);
}

} // namespace detail

template <typename ...Args>
void argv(Args &&...args)
{
    Argv tmpArgv;
    detail::buildArgv(tmpArgv, std::forward<Args>(args)...);
    detail::argvImpl(tmpArgv);
}

} // namespace pysupport

#endif // pysupport_argv_hpp_included_

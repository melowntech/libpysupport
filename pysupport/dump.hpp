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

#ifndef pysupport_dump_hpp_included_
#define pysupport_dump_hpp_included_

#include <string>
#include <sstream>

#include <boost/python.hpp>

#include "./iostreams.hpp"

namespace pysupport {

template <typename T>
std::string dump2(const T &self, const std::string &prefix)
{
    std::ostringstream os;
    self.dump(os, prefix);
    return os.str();
}

template <typename T>
std::string dump1(const T &self)
{
    return dump2<T>(self, {});
}

template <typename T>
void dumpStream2(const T &self, const OStream::pointer &os
                 , const std::string &prefix)
{
    self.dump(os->ostream(), prefix);
}

template <typename T>
void dumpStream1(const T &self, const OStream::pointer &os)
{
    self.dump(os->ostream());
}

#define PYSUPPORT_DUMPABLE(Type)                \
    .def("dump", &pysupport::dumpStream1<Type>) \
    .def("dump", &pysupport::dumpStream2<Type>) \
    .def("dump", &pysupport::dump1<Type>)       \
    .def("dump", &pysupport::dump2<Type>)

} // namespace pysupport

#endif // pysupport_dump_hpp_included_

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

#ifndef pysupport_string_hpp_included_
#define pysupport_string_hpp_included_

#include <string>

#include <boost/python/object.hpp>
#include <boost/python/extract.hpp>

namespace pysupport {

/** Convert python object to a string in utf-8.
 *
 * If type is "str" object is expected to be already in utf-8 encoding.
 *
 * Other objects are converted by calling s.encode("utf-8");
 *
 */
std::string py2utf8(const boost::python::object &s);

// inlines

inline std::string py2utf8(const boost::python::object &s)
{
    // string -> should be in utf-8
    boost::python::extract<std::string> string(s);
    if (string.check()) { return string; }

    // expect uniciode -> encode into utf-8
    return boost::python::extract<std::string>
        (s.attr("encode")("utf-8"));
}

} // namespace pysupport

#endif // pysupport_string_hpp_included_

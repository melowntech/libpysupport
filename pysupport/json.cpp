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

#include <string>
#include <vector>

#include <boost/python/slice.hpp>
#include <boost/python/object_slices.hpp>

#include "dbglog/dbglog.hpp"

#include "string.hpp"
#include "json.hpp"

namespace bp = boost::python;

namespace pysupport {

template <typename T>
class Extractor {
public:
    Extractor(const bp::object &value) : e_(value) {}
    operator bool() const { return e_.check(); }
    T get() const { return T(e_); }

private:
    bp::extract<T> e_;
};

Json::Value asJson(const bp::object &o)
{
    // Python None -> JSON null
    if (!o) { return {}; }

    // integers: TODO: check for overflow
    if (const auto &e = Extractor<Json::Int64>(o)) { return e.get(); }

    // Python double -> JSON double
    if (const auto &e = Extractor<double>(o)) { return e.get(); }
    // Python string/unicode string -> JSON string (UTF-8)
    if (Extractor<bp::str>(o)) { return py2utf8(o); }

    // Python standard sequences -> JSON array
    if (Extractor<bp::list>(o) || Extractor<bp::tuple>(o)) {
        Json::Value out(Json::arrayValue);
        for (bp::stl_input_iterator<bp::object> io(o), eo; io != eo; ++io) {
            out.append(asJson(*io));
        }
        return out;
    }

    // Python standard dictionary -> JSON object
    if (Extractor<bp::dict>(o)) {
        Json::Value out(Json::objectValue);
        for (bp::stl_input_iterator<bp::str> io(o), eo; io != eo; ++io) {
            auto key(*io);
            out[py2utf8(key)] = asJson(o[key]);
        }
        return out;
    }

    // TODO: extract type information
    LOGTHROW(err1, std::runtime_error)
        << "Unsupported Python type.";
    return {};
}

} // namespace pysupport

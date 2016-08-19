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

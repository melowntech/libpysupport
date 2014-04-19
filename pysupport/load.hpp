#ifndef pysupport_import_hpp_included_
#define pysupport_import_hpp_included_

#include <string>

#include <boost/python/object.hpp>

namespace pysupport {

/** Loads module from given data.
 */
boost::python::object load(const std::string &name, const void *data
                           , std::size_t size);

/** Shortcut for arrays.
 */
template<typename T, std::size_t size>
boost::python::object load(const std::string &name, const T(&data)[size])
{
    return load(name, data, size * sizeof(T));
}

} // namespace pysupport

#endif // pysupport_import_hpp_included_

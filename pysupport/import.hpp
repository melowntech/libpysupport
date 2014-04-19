#ifndef pysupport_import_hpp_included_
#define pysupport_import_hpp_included_

#include <boost/filesystem/path.hpp>
#include <boost/python/object.hpp>

namespace pysupport {

/** Imports module from given path.
 */
boost::python::object import(const boost::filesystem::path &path);

} // namespace pysupport

#endif // pysupport_import_hpp_included_

#ifndef pysupport_runnable_hpp_included_
#define pysupport_runnable_hpp_included_

#include <boost/python/object.hpp>

#include "utility/runnable.hpp"

namespace pysupport {

/** Runnables module from given path.
 */
boost::python::object runnable(utility::Runnable &runnable);

} // namespace pysupport

#endif // pysupport_runnable_hpp_included_

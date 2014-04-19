#ifndef pysupport_buffer_hpp_included_
#define pysupport_buffer_hpp_included_

#include <cstddef>

#include <boost/python/handle.hpp>

namespace pysupport {

boost::python::handle<> buffer(const void *data, std::size_t size);

template<typename T, std::size_t size>
boost::python::handle<> buffer(const T (&data)[size])
{
    return buffer(data, size * sizeof(T));
}

} // namespace pysupport

#endif // pysupport_buffer_hpp_included_

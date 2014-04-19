#include <boost/python/errors.hpp>

#include "buffer.hpp"

namespace pysupport {

boost::python::handle<> buffer(const void *data, std::size_t size)
{
    using namespace boost::python;

    // ugly const cast needed because of python interface...
    auto buf(PyBuffer_FromMemory(const_cast<void*>(data), size));
    if (!buf) { throw error_already_set(); }
    return handle<>(borrowed<>(buf));
}

} // namespace pysupport

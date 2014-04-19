#include <boost/python.hpp>

#include "runnable.hpp"

namespace pysupport {

namespace {
    const char *RUNNABLE_NAME("runnable");
} // namespace

class RunnableWrapper {
public:
    RunnableWrapper(boost::python::object value)
        : value_()
    {
        auto v(::PyCapsule_GetPointer(value.ptr(), RUNNABLE_NAME));
        if (!v) {
            boost::python::throw_error_already_set();
        }
        value_ = static_cast<utility::Runnable *>(v);
    }

    void operator()() {
        if (!value_ || !*value_) {
            PyErr_SetString(PyExc_KeyboardInterrupt
                            , "Process not running.");
            boost::python::throw_error_already_set();
        }
    }

private:
    utility::Runnable *value_;
};

/** Runnables module from given path.
 */
boost::python::object runnable(utility::Runnable &runnable)
{
    using namespace boost::python;
    object RunnableClass = class_<RunnableWrapper>
        ("Runnable", init<object>())
        .def("__call__", &RunnableWrapper::operator())
        ;

    auto pycaps(::PyCapsule_New(&runnable, RUNNABLE_NAME, nullptr));
    if (!pycaps) {
        throw_error_already_set();
    }
    object r = RunnableClass(object(handle<>(borrowed<>(pycaps))));

    return r;
}

} // namespace pysupport

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

#ifndef pysupport_converters_hpp_included_
#define pysupport_converters_hpp_included_

#include <new>

#include <boost/python.hpp>
#include <boost/optional.hpp>

namespace pysupport {

/** Creates to-python converter for boost::optional<T>
 *  boost::none is mapped to None
 */
template <typename T>
struct to_python_optional {
    static ::PyObject* convert(const boost::optional<T> &obj) {
        using namespace boost::python;
        if (obj) {
            return incref(object(*obj).ptr());
        }
        return incref(object().ptr());
    }
};

/** Creates from-python converter for boost::optional<T>
 *  None is mapped to boost::none
 */
template <typename T>
struct from_python_optional
{
    typedef boost::optional<T> Optional;
    typedef boost::python::converter::rvalue_from_python_stage1_data Data;
    typedef boost::python::converter::rvalue_from_python_storage<
        Optional> Storage;

    from_python_optional() {
        boost::python::converter::registry::push_back
            (&convertible, &construct, boost::python::type_id<Optional>());
    }

    // Determine if ptr can be converted in a Optional
    static void* convertible(::PyObject *ptr) {
        if (ptr == Py_None) { return ptr; }
        if (boost::python::extract<T>(ptr).check()) { return ptr; }
        return nullptr;
    }

    // Convert ptr into a Optional
    static void construct(::PyObject *ptr, Data *data) {
        // get memory
        void *storage(((Storage*) data)->storage.bytes);

        // in-place construct the Optional
        if (ptr == Py_None) {
            new (storage) Optional();
        } else {
            new (storage) Optional(boost::python::extract<T>(ptr));
        }

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

/** Convenient macro for boost::optional<T> two-way converter registration.
 */
#define PYSUPPORT_OPTIONAL(type)                                        \
    boost::python::to_python_converter<                                 \
        boost::optional<type>                                           \
    , pysupport::to_python_optional<type>>();                           \
    pysupport::from_python_optional<type>()

} // namespace pysupport

#endif // pysupport_converters_hpp_included_


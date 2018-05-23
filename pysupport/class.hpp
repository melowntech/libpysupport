
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

#ifndef pysupport_class_hpp_included_
#define pysupport_class_hpp_included_

#include <boost/python.hpp>

namespace pysupport {

/** Define readonly data member with customizable policy.
 */
template <typename Policy = boost::python::return_value_policy<
              boost::python::return_by_value>
          , typename T, typename B, typename H, typename N, typename D>
void def_readonly(boost::python::class_<T, B, H, N> &cls
                  , const char *name, D T::*pm)
{
    using namespace boost::python;
    cls.add_property(name, make_getter(pm, Policy()));
}

/** Define read/write data member with customizable policy.
 */
template <typename Policy = boost::python::return_value_policy<
              boost::python::return_by_value>
          , typename T, typename B, typename H, typename N, typename D>
void def_readwrite(boost::python::class_<T, B, H, N> &cls
                   , const char *name, D T::*pm)
{
    using namespace boost::python;
    cls.add_property(name, make_getter(pm, Policy())
                     , make_setter(pm, Policy()));
}

} // namespace pysupport

#endif // pysupport_class_hpp_included_

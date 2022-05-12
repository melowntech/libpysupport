#ifndef boost_python_definitions_hpp_included_
#define boost_python_definitions_hpp_included_

#ifdef _WIN32
#include <cmath>
#include <cstdio>
namespace std { 
    template<typename... Ts> inline auto _copysign(Ts... args) { return ::std::copysign(args...); } 
    template<typename... Ts> inline auto _vsnprintf(Ts... args) { return ::std::vsnprintf(args...); } 
    template<typename... Ts> inline auto _snprintf(Ts... args) { return ::std::snprintf(args...); } 
}
#endif

#endif
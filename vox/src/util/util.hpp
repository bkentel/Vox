#pragma once
#ifndef VOX_UTIL_UTIL_HPP
#define VOX_UTIL_UTIL_HPP

#include <functional>

namespace vox {
    namespace util {

        struct on_scope_exit {
            typedef ::std::function<void()> function;

            explicit on_scope_exit(function f)
                : f(f)
            {
            }

            ~on_scope_exit() throw() { //TODO
                try {
                    f();
                } catch (...) {
                    assert(0); //TODO
                }
            }

            function f;
        }; //struct on_scope_exit

    } //namespace util
} //namespace vox

#endif //VOX_UTIL_UTIL_HPP

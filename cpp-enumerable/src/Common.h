#pragma once

#include <coroutine>
#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <type_traits>

namespace cppe {

    template<typename Func, typename TReturn, typename... TArgs>
    concept IsFunc = std::invocable<Func, TArgs...>&&
        requires (Func&& fn, TArgs&&... args) { { fn(std::forward<TArgs>(args)...) } -> std::convertible_to<TReturn>; };
}
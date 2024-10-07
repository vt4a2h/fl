#pragma once

namespace fl {

template <class T>
struct Applicative {
    template<class V>
    [[nodiscard]] constexpr T pure(V &&) const;

    template<class F>
    [[nodiscard]] T ap(F &&) const;
};

} // namespace fl

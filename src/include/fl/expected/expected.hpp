//
// MIT License
//
// Copyright (c) 2024-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include <variant>

namespace fl {

namespace detail
{

template<class Value>
concept CorrectValue =
std::is_default_constructible_v<std::remove_cvref_t<Value>> || std::is_void_v<std::remove_cvref_t<Value>>;

template<class Value, class Error>
concept ValueAndErrorHaveDifferentTypes = !std::is_same_v<std::remove_cvref_t<Value>, std::remove_cvref_t<Error>>;

template<class From, class To>
concept ImplicitlyConvertable = std::is_convertible_v<std::remove_cvref_t<From>, std::remove_cvref_t<To>>;

template<class Value, class Error>
concept CannotCreateFromEachOther = !ImplicitlyConvertable<Value, Error> && !ImplicitlyConvertable<Error, Value>;

template<class Error>
concept NonVoidError = !std::is_void_v<std::remove_cvref_t<Error>>;

} // namespace detail

template <detail::CorrectValue Value, detail::NonVoidError Error>
    requires (detail::ValueAndErrorHaveDifferentTypes<Value, Error>) &&
             (detail::CannotCreateFromEachOther<Value, Error>)
struct expected;

namespace detail
{
struct monostate{};

template<class>
constexpr bool is_expected = false;

template<class V, class E>
constexpr bool is_expected<fl::expected<V, E>> = true;

template <class AndThenF, class Error, class ...Args>
concept CorrectAndThenFunction = requires {
    requires std::is_invocable_v<AndThenF, Args...>;
    requires is_expected<std::invoke_result_t<AndThenF, Args...>>;
    requires std::is_same_v<typename std::invoke_result_t<AndThenF, Args...>::error_t, Error>;
};

template <class OrElseF, class Value, class ...Args>
concept CorrectOrElseFunction = requires {
    requires std::is_invocable_v<OrElseF, Args...>;
    requires is_expected<std::invoke_result_t<OrElseF, Args...>>;
    requires std::is_same_v<typename std::invoke_result_t<OrElseF, Args...>::value_t, Value>;
};

template <class TransformF, class ...Args>
concept CorrectTransformFunction = requires {
    requires std::is_invocable_v<TransformF, Args...>;
    requires !is_expected<std::invoke_result_t<TransformF, Args...>>;
};

template <class NewType, class ValueType, class ErrorType>
concept ReBindable =
    ImplicitlyConvertable<ValueType, NewType> ||
    (ImplicitlyConvertable<ErrorType, NewType> && !std::is_void_v<NewType>);

template <class Value>
using ValueOrMonostate = std::conditional_t<std::is_void_v<std::remove_cvref_t<Value>>, monostate, Value>;

template <class Value>
using VoidIfMonostate = std::conditional_t<std::is_same_v<monostate, Value>, std::void_t<>, Value>;

template <class Error, class Arg>
concept SameError =
    is_expected<std::remove_cvref_t<Arg>> && std::is_same_v<typename std::remove_cvref_t<Arg>::error_t, Error>;

template <class Error, class ...Args>
concept ValidApArgs =
    (... && (!is_expected<std::remove_cvref_t<Args>> || SameError<Error, std::remove_cvref_t<Args>>)
    );

} // namespace detail

struct bind_front_t{};

template<class T>
concept is_expected = detail::is_expected<std::remove_cvref_t<T>>;

template <detail::CorrectValue Value, detail::NonVoidError Error>
    requires (detail::ValueAndErrorHaveDifferentTypes<Value, Error>) &&
             (detail::CannotCreateFromEachOther<Value, Error>)
struct expected : public std::variant<std::remove_cvref_t<detail::ValueOrMonostate<Value>>, std::remove_cvref_t<Error>>
{
    using variant_self_t = std::variant<std::remove_cvref_t<detail::ValueOrMonostate<Value>>, std::remove_cvref_t<Error>>;
    using value_t = detail::VoidIfMonostate<std::variant_alternative_t<0, variant_self_t>>;
    using error_t = std::variant_alternative_t<1, variant_self_t>;

    using variant_self_t::variant_self_t;

    [[nodiscard]] constexpr bool has_value() const { return std::holds_alternative<detail::ValueOrMonostate<Value>>(*this); }
    [[nodiscard]] constexpr bool has_error() const { return std::holds_alternative<Error>(*this); }

    constexpr explicit operator bool() const { return has_value(); }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectAndThenFunction<F, error_t, value_t, Args...>)
    [[nodiscard]] constexpr auto and_then(this Self&& self, F &&f, Args &&...back_args)
            noexcept (std::is_nothrow_invocable_v<F, value_t, Args...>)
        -> std::invoke_result_t<F, value_t, Args...>
    {
        if (self.has_value()) {
            return std::invoke(
                std::forward<F>(f), std::get<value_t>(std::forward<Self>(self)), std::forward<Args>(back_args)...);
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectAndThenFunction<F, error_t, Args..., value_t>)
    [[nodiscard]] constexpr auto and_then(this Self&& self, bind_front_t, F &&f, Args &&...back_args)
        noexcept (std::is_nothrow_invocable_v<F, Args..., value_t>)
        -> std::invoke_result_t<F, Args..., value_t>
    {
        if (self.has_value()) {
            return std::invoke(
                std::forward<F>(f), std::forward<Args>(back_args)..., std::get<value_t>(std::forward<Self>(self)));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (std::is_void_v<value_t>) &&
                 (detail::CorrectAndThenFunction<F, error_t, Args...>)
    [[nodiscard]] constexpr auto and_then(this Self&& self, F &&f, Args &&...back_args)
            noexcept (std::is_nothrow_invocable_v<F, error_t, Args...>)
        -> std::invoke_result_t<F, Args...>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::forward<Args>(back_args)...);
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectOrElseFunction<F, value_t, error_t, Args...>)
    [[nodiscard]] constexpr auto or_else(this Self&& self, F &&f, Args &&...args) noexcept
        -> std::invoke_result_t<F, error_t, Args...>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::get<error_t>(std::forward<Self>(self)), std::forward<Args>(args)...);
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectOrElseFunction<F, value_t, Args..., error_t>)
    [[nodiscard]] constexpr auto or_else(this Self&& self, bind_front_t, F &&f, Args &&...args) noexcept
        -> std::invoke_result_t<F, Args..., error_t>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)..., std::get<error_t>(std::forward<Self>(self)));
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectTransformFunction<F, value_t, Args...>)
    [[nodiscard]] constexpr auto transform(this Self&& self, F &&f, Args &&...args) noexcept
        -> expected<std::invoke_result_t<F, value_t, Args...>, error_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::get<value_t>(std::forward<Self>(self)), std::forward<Args>(args)...);
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectTransformFunction<F, Args..., value_t>)
    [[nodiscard]] constexpr auto transform(this Self&& self, bind_front_t, F &&f, Args &&...args) noexcept
    -> expected<std::invoke_result_t<F, Args..., value_t>, error_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)..., std::get<value_t>(std::forward<Self>(self)));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (std::is_void_v<value_t>) &&
                 (detail::CorrectTransformFunction<F, Args...>)
    [[nodiscard]] constexpr auto transform(this Self&& self, F &&f, Args &&...args) noexcept
        -> expected<std::invoke_result_t<F, Args...>, error_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectTransformFunction<F, error_t, Args...>)
    [[nodiscard]] constexpr auto transform_error(this Self&& self, F &&f, Args &&...args) noexcept
        -> expected<value_t, std::invoke_result_t<F, error_t, Args...>>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::get<error_t>(std::forward<Self>(self)), std::forward<Args>(args)...);
        }
    }

    template<class Self, class F, class ...Args>
        requires (detail::CorrectTransformFunction<F, Args..., error_t>)
    [[nodiscard]] constexpr auto transform_error(this Self&& self, bind_front_t, F &&f, Args &&...args) noexcept
        -> expected<value_t, std::invoke_result_t<F, Args..., error_t>>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)..., std::get<error_t>(std::forward<Self>(self)));
        }
    }

    template <class Self, class F, detail::ValidApArgs<error_t> ...Args>
    [[nodiscard]] constexpr auto ap(this Self&& /*self*/, F &&/*f*/, Args &&.../*args*/) noexcept
        /* -> */
    {

    }

    template<detail::ReBindable<value_t, error_t> NewType, class Self>
    [[nodiscard]] constexpr auto rebind(this Self&& self) noexcept
        -> expected<std::conditional_t<detail::ImplicitlyConvertable<value_t, NewType>, NewType, value_t>,
                    std::conditional_t<detail::ImplicitlyConvertable<error_t , NewType>, NewType, error_t>>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }
};

} // fl
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
std::is_default_constructible_v<std::decay_t<Value>> || std::is_void_v<std::decay_t<Value>>;

template<class Value, class Error>
concept ValueAndErrorHaveDifferentTypes = !std::is_same_v<std::decay_t<Value>, std::decay_t<Error>>;

template<class From, class To>
concept ImplicitlyConvertable = std::is_convertible_v<std::decay_t<From>, std::decay_t<To>>;

template<class Value, class Error>
concept CannotCreateFromEachOther = !ImplicitlyConvertable<Value, Error> && !ImplicitlyConvertable<Error, Value>;

template<class Error>
concept NonVoidError = !std::is_void_v<std::decay_t<Error>>;

} // namespace detail

template <detail::CorrectValue Value, detail::NonVoidError Error>
requires (detail::ValueAndErrorHaveDifferentTypes<Value, Error>) &&
(detail::CannotCreateFromEachOther<Value, Error>)
struct expected;

namespace detail
{

template<class>
constexpr bool is_expected = false;

template<class V, class E>
constexpr bool is_expected<fl::expected<V, E>> = true;

template<class>
constexpr bool is_optional = false;

template<class V>
constexpr bool is_optional<std::optional<V>> = true;

} // namespace detail

struct bind_front_t{};

template<class T>
concept IsExpected = detail::is_expected<std::decay_t<T>>;

namespace detail
{
struct monostate{};

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
using ValueOrMonostate = std::conditional_t<std::is_void_v<std::decay_t<Value>>, monostate, Value>;

template <class Value>
using VoidIfMonostate = std::conditional_t<std::is_same_v<monostate, Value>, std::void_t<>, Value>;

template <class Error, class Arg>
concept SameError =
    is_expected<std::decay_t<Arg>> && std::is_same_v<typename std::decay_t<Arg>::error_t, Error>;

template <class Result, class Error>
concept NotExpectedOrSameError = !is_expected<std::decay_t<Result>> || SameError<Error, std::decay_t<Result>>;

template <class Error, class ...Args>
concept ValidApArgs = (... && NotExpectedOrSameError<Args, Error>);

template <typename Arg>
struct UnwarpOrForwardImpl {
    using type = Arg;
};

template <typename Arg>
    requires (is_expected<std::decay_t<Arg>>)
struct UnwarpOrForwardImpl<Arg> {
    using type = typename std::decay_t<Arg>::value_t;
};

// NOTE: unfortunately, conditional_t doest work here, even with type_identity :(
template <class Arg>
using UnwarpOrForward = typename UnwarpOrForwardImpl<Arg>::type;

template <class F, class Error, class ...Args>
concept ApInvocable = requires {
    requires ValidApArgs<Error, Args...>;
    requires std::is_invocable_v<F, UnwarpOrForward<Args>...>;
    requires NotExpectedOrSameError<std::invoke_result_t<F, UnwarpOrForward<Args>...>, Error>;
};

template <class E, class Arg>
constexpr auto operator <<(std::optional<E> optErr, Arg &&arg) noexcept -> std::optional<E>
{
    if (optErr) {
        return optErr;
    }

    if constexpr (is_expected<std::decay_t<Arg>>) {
        if (arg.has_error()) {
            return std::make_optional<E>(std::get<typename std::decay_t<Arg>::error_t>(std::forward<Arg>(arg)));
        }
    }

    return std::nullopt;
}

// NOTE: we can combine errors as well. This will require "combine(T l, T r) -> T" (i.e. semigroup-like)
template <class Error, class ...Args>
    requires (ValidApArgs<Error, Args...>)
constexpr std::optional<Error> firstError(const Args&...args)
{
    return (std::optional<Error>{} << ... << args);
}

template <class T>
concept CanCombine = requires(T a1, T a2) {
    { combine(a1, a2) } -> std::same_as<T>;
};

template <class E, class Arg>
    requires (CanCombine<std::optional<std::decay_t<E>>> &&
              CanCombine<std::decay_t<E>>)
constexpr auto operator <<(std::optional<E> optErr, Arg &&arg) noexcept -> std::optional<E>
{
    if constexpr (is_expected<std::decay_t<Arg>>) {
        if (arg.has_error()) {
            return combine(std::move(optErr),
                           std::make_optional<E>(std::get<typename std::decay_t<Arg>::error_t>(std::forward<Arg>(arg))));
        }
    }

    return optErr;
}

template <class Error, class ...Args>
    requires (ValidApArgs<Error, Args...>)
constexpr std::optional<Error> combineErrors(const Args&...args)
{
    return (std::optional<Error>{} << ... << args);
}

template <class F, class ...Args>
using JustInvocableResult = std::decay_t<std::invoke_result_t<F, UnwarpOrForward<Args>...>>;

// NOTE: unfortunately, conditional_t doest work here, even with type_identity :(
template <class Arg, class Error>
struct WarpIntoExpectedOrForward {
    using type = fl::expected<Arg, Error>;
};

template <class Arg, class Error>
    requires (is_expected<std::decay_t<Arg>>)
struct WarpIntoExpectedOrForward<Arg, Error> {
    using type = Arg;
};

template <class F, class Error, class ...Args>
using ApInvocableResult = typename WarpIntoExpectedOrForward<JustInvocableResult<F, Args...>, Error>::type;

template <class Arg>
constexpr decltype(auto) unwrapOrForward(Arg &&arg)
{
    return std::forward<Arg>(arg);
}

template <class Arg>
    requires (is_expected<std::decay_t<Arg>>)
constexpr decltype(auto) unwrapOrForward(Arg &&arg)
{
    return std::get<typename std::decay_t<Arg>::value_t>(std::forward<Arg>(arg));
}

template<class>
constexpr bool dependent_false = false;

template <class E>
concept CustomValueHandlerFound = requires {
    requires is_expected<std::decay_t<E>>;
    handle_bad_value(typename std::decay_t<E>::error_t{});
};

template <class E>
concept CustomErrorHandlerFound = requires {
    requires is_expected<std::decay_t<E>>;
    handle_bad_error(typename std::decay_t<E>::value_t{});
};

template <class>
constexpr void default_handle_bad_value()
{
    if (std::is_constant_evaluated()) {
        throw "Expected object contains error";
    } else {
        std::terminate();
    }
}

template <class>
constexpr void default_handle_bad_error()
{
    if (std::is_constant_evaluated()) {
        throw "Expected object contains value";
    } else {
        std::terminate();
    }
}

} // namespace detail

template <detail::CorrectValue Value, detail::NonVoidError Error>
    requires (detail::ValueAndErrorHaveDifferentTypes<Value, Error>) &&
             (detail::CannotCreateFromEachOther<Value, Error>)
struct expected : public std::variant<std::decay_t<detail::ValueOrMonostate<Value>>, std::decay_t<Error>>
{
    using variant_self_t = std::variant<std::decay_t<detail::ValueOrMonostate<Value>>, std::decay_t<Error>>;
    using value_t = detail::VoidIfMonostate<std::variant_alternative_t<0, variant_self_t>>;
    using error_t = std::variant_alternative_t<1, variant_self_t>;

    using variant_self_t::variant_self_t;

    [[nodiscard]] constexpr bool has_value() const { return std::holds_alternative<detail::ValueOrMonostate<Value>>(*this); }
    [[nodiscard]] constexpr bool has_error() const { return std::holds_alternative<Error>(*this); }

    template<class Self>
        requires (!std::is_void_v<value_t>)
    [[nodiscard]] constexpr auto&& value(this Self&& self)
    {
        if (self.has_error()) {
            if constexpr (detail::CustomValueHandlerFound<Self>) {
                handle_bad_value(std::get<error_t>(std::forward<Self>(self)));
            } else {
                detail::default_handle_bad_value<Self>();
            }
        }

        return std::get<value_t>(std::forward<Self>(self));
    }

    template<class Self>
        requires (std::is_void_v<value_t>)
    constexpr void value(this Self&& self)
    {
        if (self.has_error()) {
            if constexpr (detail::CustomValueHandlerFound<Self>) {
                handle_bad_value(std::get<error_t>(std::forward<Self>(self)));
            } else {
                detail::default_handle_bad_value<Self>();
            }
        }
    }

    template<class Self>
        requires (!std::is_void_v<error_t>)
    [[nodiscard]] constexpr auto&& error(this Self&& self)
    {
        if (self.has_value()) {
            if constexpr (detail::CustomErrorHandlerFound<Self>) {
                handle_bad_error(std::get<value_t>(std::forward<Self>(self)));
            } else {
                detail::default_handle_bad_error<Self>();
            }
        }

        return std::get<error_t>(std::forward<Self>(self));
    }

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

    template <class Self, class F, class ...Args>
        requires (detail::ApInvocable<F, error_t, value_t, Args...>)
    [[nodiscard]] constexpr auto ap(this Self&& self, F &&f, Args &&...args)
        noexcept(std::is_nothrow_invocable_v<F, value_t, detail::UnwarpOrForward<Args>...>)
        -> detail::ApInvocableResult<F, error_t, value_t, Args...>
    {
        if (auto firstError = detail::firstError<error_t>(self, args...)) {
            return std::move(firstError).value();
        } else {
            return std::invoke(
                std::forward<F>(f),
                    std::get<value_t>(std::forward<Self>(self)),
                        detail::unwrapOrForward(std::forward<Args>(args))...);
        }
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
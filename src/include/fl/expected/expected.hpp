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

template <class Value_, class Error_>
class Expected;

template <class Error_>
struct Unexpected;

namespace concepts {

namespace details
{

template<class>
constexpr bool is_expected = false;

template<class V, class E>
constexpr bool is_expected<fl::Expected<V, E>> = true;

template<class>
constexpr bool is_unexpected = false;

template<class E>
constexpr bool is_unexpected<fl::Unexpected<E>> = true;

template <class Value_, class AnotherValue_, class AnotherError_>
concept CannotConstructExpectedOrValueFrom =
    std::is_same_v<std::remove_cvref_t<Value_>, bool> ||
        (!std::is_constructible_v<Value_, fl::Expected<AnotherValue_, AnotherError_>&> &&
         !std::is_constructible_v<Value_, fl::Expected<AnotherValue_, AnotherError_>> &&
         !std::is_constructible_v<Value_, const fl::Expected<AnotherValue_, AnotherError_>&> &&
         !std::is_constructible_v<Value_, const fl::Expected<AnotherValue_, AnotherError_>> &&
         !std::is_convertible_v<fl::Expected<AnotherValue_, AnotherError_>&, Value_> &&
         !std::is_convertible_v<fl::Expected<AnotherValue_, AnotherError_>, Value_> &&
         !std::is_convertible_v<const fl::Expected<AnotherValue_, AnotherError_>&, Value_> &&
         !std::is_convertible_v<const fl::Expected<AnotherValue_, AnotherError_>, Value_>);

template <class Error_, class AnotherValue_, class AnotherError_>
concept CannotConstructUnexpectedFrom =
    !std::is_constructible_v<fl::Unexpected<Error_>, fl::Expected<AnotherValue_, AnotherError_>&> &&
    !std::is_constructible_v<fl::Unexpected<Error_>, fl::Expected<AnotherValue_, AnotherError_>> &&
    !std::is_constructible_v<fl::Unexpected<Error_>, const fl::Expected<AnotherValue_, AnotherError_>&> &&
    !std::is_constructible_v<fl::Unexpected<Error_>, const fl::Expected<AnotherValue_, AnotherError_>>;

} // namespace details

template<class T>
concept IsExpected = details::is_expected<std::remove_cvref_t<T>>;

template<class T>
concept IsUnexpected = details::is_unexpected<std::remove_cvref_t<T>>;

} // fl

template <class Error_>
struct Unexpected
{
public: // Types
    using Error = Error_;

    template<class E_ = Error_>
        requires
            (!std::is_same_v<std::remove_cvref_t<E_>, Unexpected>) &&
            (!std::is_same_v<std::remove_cvref_t<E_>, std::in_place_t>) &&
            std::is_constructible_v<Error_, E_>
    constexpr explicit Unexpected(E_&& error) noexcept(std::is_nothrow_constructible_v<E_, Error_>)
        : m_error(std::forward<E_>(error))
    {}

    template<class... Args>
        requires (std::is_constructible_v<Error_, Args...>)
    constexpr explicit Unexpected(std::in_place_t, Args&&... args )
        noexcept(std::is_nothrow_constructible_v<Error_, Args...>)
        : m_error(std::forward<Args>(args)...)
    {}

    template<class U, class... Args >
        requires (std::is_constructible_v<Error_, std::initializer_list<U>&, Args...>)
    constexpr explicit Unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args )
        noexcept(std::is_nothrow_constructible_v<Error_, std::initializer_list<U>&, Args...>)
        : m_error(il, std::forward<Args>(args)...)
    {}

    constexpr Unexpected(const Unexpected& src) noexcept(std::is_nothrow_copy_constructible_v<Error_>)
        requires (std::is_copy_constructible_v<Error_>)
        : m_error(src.m_error)
    {}
    constexpr Unexpected(Unexpected &&src) noexcept(std::is_nothrow_move_assignable_v<Error_>)
        requires (std::is_move_constructible_v<Error_>)
        : m_error(std::move(src.m_error))
    {}

    template <class Self>
    constexpr auto&& error(this Self&& self) noexcept
    {
        return std::forward<Self>(self).m_error;
    }

    constexpr auto operator<=>(const Unexpected&) const = default;

private:
    Error_ m_error;
};

template <class Error_>
Unexpected(Error_) -> Unexpected<Error_>;

template <class Value_, class Error_>
class Expected
{
public: // Types
    using Value = Value_;
    using Error = Error_;
    using Data = std::variant<Value, Error_>;

public: // Methods
    constexpr Expected() noexcept(std::is_nothrow_default_constructible_v<Value_>)
        requires (std::is_default_constructible_v<Value_>)
    = default;

    constexpr Expected(const Expected& other)
        noexcept (std::is_nothrow_copy_constructible_v<Data>)
        requires (std::is_copy_constructible_v<Data>)
    = default;

    constexpr Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<Data>)
        requires (std::is_move_constructible_v<Data>)
    = default;

    template<class AnotherValue_, class AnotherError_>
        requires
            (std::is_constructible_v<Value_, const AnotherValue_&>) &&
            (std::is_constructible_v<Error_, const AnotherError_&>) &&
            (concepts::details::CannotConstructExpectedOrValueFrom<Value_, AnotherValue_, AnotherError_>) &&
            (concepts::details::CannotConstructUnexpectedFrom<Error_, AnotherValue_, AnotherError_>)
    constexpr explicit(!std::is_convertible_v<const AnotherValue_&, Value_> || !std::is_convertible_v<const AnotherError_&, Error_>)
        Expected(const Expected<AnotherValue_, AnotherError_>& other)
    noexcept (std::is_nothrow_constructible_v<Value_, const AnotherValue_&> && std::is_nothrow_constructible_v<Error_, const AnotherError_&>)
        : m_data(other.hasValue() ? Data{*other} : Data{other.error()})
    {}

    template<class AnotherValue_, class AnotherError_>
        requires
            (std::is_constructible_v<Value_, AnotherValue_>) &&
            (std::is_constructible_v<Error_, AnotherError_>)  &&
            (concepts::details::CannotConstructExpectedOrValueFrom<Value_, AnotherValue_, AnotherError_>) &&
            (concepts::details::CannotConstructUnexpectedFrom<Error_, AnotherValue_, AnotherError_>)
    constexpr explicit(!std::is_convertible_v<AnotherValue_, Value_> || !std::is_convertible_v<AnotherError_, Error_>)
        Expected(Expected<AnotherValue_, AnotherError_>&& other)
    noexcept (std::is_nothrow_constructible_v<Value_, AnotherValue_> && std::is_nothrow_constructible_v<Error_, AnotherError_>)
        : m_data(other.hasValue() ? Data{std::forward<AnotherValue_>(*other)} : Data{std::forward<AnotherError_>(other.error())})
    {}

    template <class AnotherValue = Value_>
        requires
            (!std::is_same_v<std::remove_cvref_t<AnotherValue>, std::in_place_t>) &&
            (!std::is_same_v<Expected<Value_, Error_>, std::remove_cvref_t<AnotherValue>>) &&
            (std::is_constructible_v<Value_, AnotherValue>) &&
            (!concepts::IsUnexpected<AnotherValue>) &&
            (!std::is_same_v<std::remove_cvref_t<Value_>, bool> || !concepts::IsExpected<AnotherValue>)
    constexpr explicit(!std::is_convertible_v<AnotherValue, Value_>) Expected(AnotherValue&& v)
        noexcept (std::is_nothrow_constructible_v<Value_, AnotherValue>)
        : m_data(std::forward<AnotherValue>(v))
    {}

    template<class AnotherError>
        requires (std::is_constructible_v<const AnotherError&, Error>)
    constexpr explicit(!std::is_convertible_v<const AnotherError&, Error>) Expected(const Unexpected<AnotherError> &unexpected)
        noexcept (std::is_nothrow_convertible_v<const AnotherError&, Error>)
            : m_data(unexpected.error())
    {}

    template<class AnotherError>
        requires (std::is_constructible_v<AnotherError, Error>)
    constexpr explicit(!std::is_convertible_v<AnotherError, Error>) Expected(Unexpected<AnotherError> &&unexpected)
        noexcept (std::is_nothrow_convertible_v<AnotherError, Error>)
            : m_data(std::move(unexpected).error())
    {}

    [[nodiscard]] constexpr bool hasValue() const {
        return std::holds_alternative<Value_>(m_data);
    }

    [[nodiscard]] explicit constexpr operator bool() const {
        return hasValue();
    }

    template <class Self>
        requires (!std::is_same_v<Value_, void>)
    constexpr auto&& value(this Self&& self) noexcept {
        // TODO: implement a handler {
        // assert(self.hasValue());
        // }

        return std::get<Value_>(std::forward<Self>(self).m_data);
    }

    // TODO: requires certain ctors etc
    template <class Self>
    requires (std::is_same_v<Value_, void>)
    constexpr void value(this Self&& /*self*/) noexcept {
        // TODO: implement a handler {
        // assert(self.hasValue());
        // }
    }

    [[nodiscard]] constexpr bool hasError() const {
        return std::holds_alternative<Error_>(m_data);
    }

    template <class Self>
    constexpr auto&& error(this Self&& self) noexcept {
        // TODO: implement a handler {
        // assert(self.hasValue());
        // }

        return std::get<Error_>(std::forward<Self>(self).m_data);
    }

    template <class Self>
    constexpr auto&& as_variant(this Self&& self) noexcept
    {
        return std::forward_like<Self>(self.m_data);
    }

    constexpr friend bool operator ==(const Expected &lhs, const Expected &rhs) = default;

private:
    Data m_data;
};

namespace experimental {

template <class Value>
concept CorrectValue =
    std::is_default_constructible_v<std::remove_cvref_t<Value>> || std::is_void_v<std::remove_cvref_t<Value>>;

template <class Value, class Error>
concept ValueAndErrorHaveDifferentTypes = !std::is_same_v<std::remove_cvref_t<Value>, std::remove_cvref_t<Error>>;

template <class From, class To>
concept ImplicitlyConvertable = std::is_convertible_v<std::remove_cvref_t<From>, std::remove_cvref_t<To>>;

template <class Value, class Error>
concept CannotCreateFromEachOther = !ImplicitlyConvertable<Value, Error> && !ImplicitlyConvertable<Error, Value>;

template <class Error>
concept NonVoidError = !std::is_void_v<std::remove_cvref_t<Error>>;

template <CorrectValue Value, NonVoidError Error>
    requires ValueAndErrorHaveDifferentTypes<Value, Error> &&
             CannotCreateFromEachOther<Value, Error>
struct expected;

namespace details {
    template<class>
    constexpr bool is_expected = false;

    template<class V, class E>
    constexpr bool is_expected<expected<V, E>> = true;

    struct monostate{};

    template <class ...Args>
    concept AtLeastONeArgIsRequired = sizeof ...(Args) >= 1;

    template<typename T>
    inline constexpr bool always_false_v = false;

    template <class T>
    decltype(auto) copy_if_non_movable(T &&v)
    {
        if constexpr (!std::is_const_v<std::remove_reference_t<T>> &&
                      !std::is_rvalue_reference_v<T> &&
                      !std::is_move_constructible_v<std::remove_cvref_t<T>>) {
            return static_cast<std::remove_cvref_t<T>>(v);
        } else {
            return std::forward<T>(v);
        }
    }

    template <class F, AtLeastONeArgIsRequired ...Args>
    [[nodiscard]] auto bind_back(F &&f, Args &&...args) noexcept
    {
        return [...b_args = copy_if_non_movable(std::forward<Args>(args)), b_f = std::forward<F>(f)]<class ...F_Args>(F_Args &&...f_args) {
            return std::invoke(b_f, std::forward<F_Args>(f_args)..., b_args...);
        };
    }

    template <class F, AtLeastONeArgIsRequired ...Args>
    [[nodiscard]] auto bind_front(F &&f, Args &&...args) noexcept
    {
        return [...f_args = copy_if_non_movable(std::forward<Args>(args)), b_f = std::forward<F>(f)]<class ...B_Args>(B_Args &&...b_args) {
            return std::invoke(b_f, std::forward_like<Args>(f_args)..., b_args...);
        };
    }
} // namespace details

template<class T>
concept is_expected = details::is_expected<std::remove_cvref_t<T>>;

template <class AndThenF, class Error, class ...Args>
concept CorrectAndThenFunction = requires {
    requires std::is_invocable_v<AndThenF, Args...>;
    requires is_expected<std::invoke_result_t<AndThenF, Args...>>;
    requires std::is_same_v<typename std::invoke_result_t<AndThenF, Args...>::error_t, Error>;
};

template <class OrElseF, class Value, class Error>
concept CorrectOrElseFunction = requires {
    requires std::is_invocable_v<OrElseF, Error>;
    requires is_expected<std::invoke_result_t<OrElseF, Error>>;
    requires std::is_same_v<typename std::invoke_result_t<OrElseF, Error>::value_t, Value>;
};

template <class TransformF, class ...Args>
concept CorrectTransformFunction = requires {
    requires std::is_invocable_v<TransformF, Args...>;
    requires !is_expected<std::invoke_result_t<TransformF, Args...>>;
};

template <class TransformF, class Error>
concept CorrectTransformErrorFunction = requires {
    requires std::is_invocable_v<TransformF, Error>;
    requires !is_expected<std::invoke_result_t<TransformF, Error>>;
};

template <class NewType, class ValueType, class ErrorType>
concept ReBindable = ImplicitlyConvertable<ValueType, NewType> || ImplicitlyConvertable<ErrorType, NewType>;

template <class Value>
using ValueOrMonostate = std::conditional_t<std::is_void_v<std::remove_cvref_t<Value>>, details::monostate, Value>;

template <class Value>
using VoidIfMonostate = std::conditional_t<std::is_same_v<details::monostate, Value>, std::void_t<>, Value>;

template <CorrectValue Value, NonVoidError Error>
    requires ValueAndErrorHaveDifferentTypes<Value, Error> &&
             CannotCreateFromEachOther<Value, Error>
struct expected : public std::variant<std::remove_cvref_t<ValueOrMonostate<Value>>, std::remove_cvref_t<Error>>
{
    using variant_self_t = std::variant<std::remove_cvref_t<ValueOrMonostate<Value>>, std::remove_cvref_t<Error>>;
    using value_t = VoidIfMonostate<std::variant_alternative_t<0, variant_self_t>>;
    using error_t = std::variant_alternative_t<1, variant_self_t>;

    using variant_self_t::variant_self_t;

    [[nodiscard]] constexpr bool has_value() const { return std::holds_alternative<ValueOrMonostate<Value>>(*this); }
    [[nodiscard]] constexpr bool has_error() const { return std::holds_alternative<Error>(*this); }

    constexpr explicit operator bool() const { return has_value(); }

    template<class Self, CorrectAndThenFunction<error_t, value_t> F>
    [[nodiscard]] constexpr auto and_then(this Self&& self, F &&f) noexcept -> std::invoke_result_t<F, value_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::get<value_t>(std::forward<Self>(self)));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, CorrectAndThenFunction<error_t> F>
        requires (std::is_void_v<value_t>)
    [[nodiscard]] constexpr auto and_then(this Self&& self, F &&f) noexcept -> std::invoke_result_t<F>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, CorrectOrElseFunction<value_t, error_t> F>
    [[nodiscard]] constexpr auto or_else(this Self&& self, F &&f) noexcept -> std::invoke_result_t<F, error_t>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::get<error_t>(std::forward<Self>(self)));
        }
    }

    template<class Self, CorrectTransformFunction<value_t> F>
    [[nodiscard]] constexpr auto transform(this Self&& self, F &&f) noexcept
        -> expected<std::invoke_result_t<F, value_t>, error_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f), std::get<value_t>(std::forward<Self>(self)));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, CorrectTransformFunction<> F>
        requires (std::is_void_v<value_t>)
    [[nodiscard]] constexpr auto transform(this Self&& self, F &&f) noexcept
        -> expected<std::invoke_result_t<F>, error_t>
    {
        if (self.has_value()) {
            return std::invoke(std::forward<F>(f));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }

    template<class Self, CorrectTransformErrorFunction<error_t> F>
    [[nodiscard]] constexpr auto transform_error(this Self&& self, F &&f) noexcept
        -> expected<value_t, std::invoke_result_t<F, error_t>>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::invoke(std::forward<F>(f), std::get<error_t>(std::forward<Self>(self)));
        }
    }

    template<ReBindable<value_t, error_t> NewType, class Self>
    [[nodiscard]] constexpr auto rebind(this Self&& self) noexcept
        -> expected<std::conditional_t<ImplicitlyConvertable<value_t, NewType>, NewType, value_t>,
                    std::conditional_t<ImplicitlyConvertable<error_t , NewType>, NewType, error_t>>
    {
        if (self.has_value()) {
            return std::get<value_t>(std::forward<Self>(self));
        } else {
            return std::get<error_t>(std::forward<Self>(self));
        }
    }
};

} // experimental


} // fl
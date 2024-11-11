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

//    template< class U, class G >
//    constexpr expected( const expected<U, G>& other );
//
//    template< class U, class G >
//    constexpr expected( expected<U, G>&& other );

    template <class OtherValue = Value_>
        requires
            (!std::is_same_v<std::remove_cvref_t<OtherValue>, std::in_place_t>) &&
            (!std::is_same_v<Expected<Value_, Error_>, std::remove_cvref_t<OtherValue>>) &&
            (std::is_constructible_v<Value_, OtherValue>) &&
            (!concepts::IsUnexpected<OtherValue>) &&
            (!std::is_same_v<std::remove_cvref_t<Value_>, bool> || !concepts::IsExpected<OtherValue>)
    constexpr explicit(!std::is_convertible_v<OtherValue, Value_>) Expected(OtherValue&& v)
        noexcept (std::is_nothrow_constructible_v<Value_, OtherValue>)
        : m_data(std::forward<OtherValue>(v))
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

    constexpr friend bool operator ==(const Expected &lhs, const Expected &rhs)
    {
        return lhs.m_data == rhs.m_data;
    }

private:
    Data m_data;
};


} // fl
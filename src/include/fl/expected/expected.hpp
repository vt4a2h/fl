#include <variant>

#include <expected>

namespace fl {

namespace concepts {

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

    constexpr Unexpected(const Unexpected&) = default;
    constexpr Unexpected(Unexpected&&) = default;

    template <class Self>
    constexpr auto&& error(this Self&& self) noexcept
    {
        return std::forward<Self>(self).m_error;
    }

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
    constexpr Expected()
        requires (std::is_default_constructible_v<Value_>)
    = default;

    template<class AnotherError>
        requires (std::is_constructible_v<const AnotherError&, Error>)
    constexpr explicit(!std::is_convertible_v<const AnotherError&, Error>) Expected(const Unexpected<AnotherError> &unexpected)
        : m_data(unexpected.error())
    {}

    template<class AnotherError>
        requires (std::is_constructible_v<AnotherError, Error>)
    constexpr explicit(!std::is_convertible_v<AnotherError, Error>) Expected(Unexpected<AnotherError> &&unexpected)
        : m_data(std::move(unexpected).error())
    {}

    constexpr Expected(const Expected& other)
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
            (!std::is_same_v<std::expected<Value_, Error_>, std::remove_cvref_t<OtherValue>>) &&
            std::is_constructible_v<Value_, OtherValue>
            /* TODO:
             * Add constraints:
             * std::remove_cvref_t<U> is not a specialization of std::unexpected
             * If T is (possibly cv-qualified) bool, std::remove_cvref_t<U> is not a specialization of std::expected.
             */
    constexpr explicit(!std::is_convertible_v<OtherValue, Value_>) Expected(OtherValue&& v)
        : m_data(std::forward<OtherValue>(v))
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
    constexpr void value(this Self&& self) noexcept {
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

    template <class AnotherValue, class AnotherError>
        requires (std::equality_comparable_with<typename Expected::Data,
                                                typename Expected<AnotherError, AnotherValue>::Data>)
    constexpr friend bool operator ==(const Expected &lhs, const Expected<AnotherError, AnotherValue> &rhs)
    {
        return lhs.m_data == rhs.m_data;
    }

private:
    Data m_data;
};


} // fl
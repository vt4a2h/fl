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

public: // Methods
    constexpr Expected()
        requires (std::is_default_constructible_v<Value_>)
    = default;

    template<class AnotherError>
    constexpr explicit(!std::is_convertible_v<const AnotherError&, Error>) Expected(const Unexpected<AnotherError> &unexpected)
        requires (std::is_constructible_v<const AnotherError&, Error>)
        : m_data(unexpected.error())
    {}

    template<class AnotherError>
    constexpr explicit(!std::is_convertible_v<AnotherError, Error>) Expected(Unexpected<AnotherError> &&unexpected)
    requires (std::is_constructible_v<AnotherError, Error>)
        : m_data(std::move(unexpected).error())
    {}

    [[nodiscard]] constexpr bool hasValue() const {
        return std::holds_alternative<Value_>(m_data);
    }

    [[nodiscard]] explicit constexpr operator bool() const {
        return hasValue();
    }

    // TODO: more options
    [[nodiscard]] constexpr Value_ value() const {
        // TODO: check first
        return std::get<Value_>(m_data);
    }

    [[nodiscard]] constexpr bool hasError() const {
        // TODO: check first
        return std::holds_alternative<Error_>(m_data);
    }

    // TODO: more options
    [[nodiscard]] constexpr Error_ error() const {
        // TODO: check first
        return std::get<Error_>(m_data);
    }

private:
    std::variant<Value, Error_> m_data;
};


} // fl
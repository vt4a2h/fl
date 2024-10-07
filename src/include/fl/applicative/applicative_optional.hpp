#include <fl/applicative/applicative.hpp>
#include <fl/concepts/concepts.hpp>



namespace fl {

template <concepts::IsOptional T>
struct Applicative<T>
{
    using value_type = typename T::value_type;

    [[nodiscard]]  constexpr T pure(concepts::SameOrConstructable<value_type> auto &&v) const
    {
        return std::make_optional(value_type(std::forward<decltype(v)>(v)));
    }
};

} // namespace fl

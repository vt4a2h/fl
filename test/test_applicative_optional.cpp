#include "catch.hpp"

#include <fl/applicative/applicative_optional.hpp>


TEST_CASE("Applicative for std::optional")
{
    fl::Applicative<std::optional<int>> applicative;

    SECTION("Pure")
    {
        REQUIRE(applicative.pure(42) == std::optional<int>(42));
    }
}
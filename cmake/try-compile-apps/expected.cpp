#include <expected>
#include <string>

int main(int /*argc*/, char* /*argv*/[])
{
    using Expected = std::expected<int, std::string>;

    Expected result{0};
    Expected unxepectedResult = std::unexpected(std::string{"foo"});

    return result != unxepectedResult ? *result : -1;
}
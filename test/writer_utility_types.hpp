//
// MIT License
//
// Copyright (c) 2023-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//

#include <string>

#include <fl/semigroups/semigroup.hpp>
#include <fl/writer.hpp>

namespace writer_tests_util {

class NonCopyableString : public std::string {
public:
    NonCopyableString() = default;
    explicit NonCopyableString(const std::string& s) : std::string(s) {}

    NonCopyableString(NonCopyableString &&) noexcept = default;
    NonCopyableString &operator=(NonCopyableString &&) noexcept = default;

    explicit NonCopyableString(const NonCopyableString&) = delete;
    NonCopyableString &operator=(const NonCopyableString&) = delete;
};

class NonMovableString : public std::string {
public:
    NonMovableString() = default;
    explicit NonMovableString(const std::string& s) : std::string(s) {}
    NonMovableString(const NonMovableString&) = default;
    NonMovableString &operator=(const NonMovableString&) = default;

    explicit NonMovableString(NonMovableString &&) = delete;
    NonMovableString &operator=(NonMovableString &&) = delete;
};

} // writer_tests_util

namespace fl {

template<>
struct Semigroup<writer_tests_util::NonCopyableString> {
    [[nodiscard]]
    writer_tests_util::NonCopyableString combine(writer_tests_util::NonCopyableString&& v1, writer_tests_util::NonCopyableString&& v2) const {
        v1.reserve(v1.size() + v2.size() + 1);
        v1.append("-").append(v2);

        return v1;
    }
};

template<>
struct Semigroup<writer_tests_util::NonMovableString> {
    [[nodiscard]]
    writer_tests_util::NonMovableString combine(const writer_tests_util::NonMovableString& v1, const writer_tests_util::NonMovableString& v2) const {
        writer_tests_util::NonMovableString result;

        result.reserve(v1.size() + v2.size() + 1);
        result.append(v1).append("-").append(v2);

        return result;
    }
};

} // namespace fl

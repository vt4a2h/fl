![Build and test](https://github.com/vt4a2h/fl/actions/workflows/build-and-test.yaml/badge.svg)

# Table of contents
- [Overview](#overview)
- [How to use](#how-to-use)
- [Simple use case](#simple-use-case)
- [Customization points](#customization-points)

## Overview

This is a rather simple implementation of the Writer monad in C++. In this implementation, Writer is not really a monad,
but still can be used for logging without side effects. This is a very experimental library. It's not widely used,
not well-tested, and requires new compilers with C++23 support.

## How to use

Neither vcpkg nor Conan integrations are available now, but this is the header-only library. It means that you can
simply clone the repo, add it as a sub-folder (or even a submodule) to your project and use.

## Simple use case

There is a function for calculating factorial:
```c++
using Log = std::vector<std::string>;
using Val = std::uint64_t;
using Logger = fl::Writer<Log, Val>;

[[nodiscard]]
Logger factorial(Val i) {
    const auto mult = [&](Val v) { return v * i; };
    const auto tell = [&](Val ans) { return Logger{{fmt::format("Factorial of {} is {}", i, ans)}, ans}; };
    return (i == 0 ? Logger{{}, 1} : factorial(i - 1).transform(mult)).and_then(tell);
}
```
This function logs progress at each calculation step. Once the calculations are finished, you can get access to the 
logs:
```c++
const auto &[l, v] = factorial(value);
for (const auto &s : l) {
    logger()->info(s);
}
```

## Customization points

You can specify Semigroup for your type:
```c++
struct MyString : public std::string{ using std::string::string; };

namespace fl {

template<>
struct Semigroup<MyString> {
    [[nodiscard]]
    MyString combine(const MyString& v1, const MyString& v2) const {
        MyString result;

        result.reserve(v1.size() + v2.size() + 1);
        result.append(v1).append("\n").append(v2);

        return result;
    }
};

}
```
Then the writer can be easily used as usual:
```c++
auto customWriter = fl::Writer<MyString, int>{"foo", 1};

auto actual = customWriter.tell("bar")._log;
auto expected = MyString("foo\nbar");

REQUIRE(actual == expected);
```
It's also possible to use purely custom Semigroup with a type-erasure technique. For more details see:
`test/test_writer_any_semigroup.cpp`
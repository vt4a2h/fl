![Build and test](https://github.com/vt4a2h/fl/actions/workflows/build-and-test.yaml/badge.svg)

# Table of contents
- [Overview](#overview)
- [How to use](#how-to-use)
- [Expected](#expected)
- [Writer monad](#writer-monad)
  - [Simple use case](#simple-use-case)
  - [Customization points](#customization-points)

## Overview

This is a collection of the different functional tools for C++:
- experimental implementation of [expected](src/include/fl/expected/expected.hpp) with extended functionality 
- [writer monad](src/include/fl/writer/writer.hpp)

## How to use

There is a non-official vcpkg registry I created for this package. In order to use it, you need to:
- Create `vcpkg-configuration.json` next to the `vcpkg.json`
- Add a registry called `https://github.com/vt4a2h/fl-vcpkg-registry.git` for the dependency `fl`
- Add `fl` to the dependencies list in the `vcpkg.json`

`vcpkg-configuration.json`
```json
{
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/vt4a2h/fl-vcpkg-registry.git",
      "packages": [
        "fl"
      ],
      "baseline": "66f78b6df4a1a97e2ebe8958a910433d9439fa72"
    }
  ]
}
```
`vcpkg.json`
```json
"dependencies": [
  "fl"
],
```

Then add the following lines to the `CMakeListst.txt` file:
```cmake
find_package(fl CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE fl::fl)
```

And that's it, you can use the library now! Please, make sure that your compiler is new enough and supports C++26.

```c++
#include <fl/writer/all.hpp>

// ...

fl::Writer<std::string, int> writer;
```

NB! Do not use `https://github.com/vt4a2h/fl-vcpkg-registry.git` for any official, production-ready builds. 
In case you want, mirror it, or just copy it.

Alternatively, you can clone the repo as a submodule and include `src/include` directory.

## Expected

This experimental implementation aims to eliminate the major disadvantages of `std::expected` and introduce some useful 
functionality:
- Current implementation is based on std::variant
  - std::unexpected is not required
  - types of error and value must be different
- Value and error accessors can be customised
  - no throw and no UB by default
  - action on failure can be customised (std::terminate by default)
- All monadic operations come with built-in bind_back and bind_front
- One more monadic operation is added (ap -- applicative-like)

How to use:
```c++
#include <fl/expected/expected.hpp>

using Expected = expected<int, std::string>;

const Expected expected = /* ... */;

/* ... */ = expected.and_then([](const int& value) -> Expected {
    // Return result
    return 42;
    
    // Return error
    return "";
})
.or_else([](const std::string& error) -> Expected {
    // Handle error...
    return {};
});
```

See [tests](test/expected) for more detailed examples.

## Writer monad

In this implementation, Writer is not really a monad,
but still can be used for logging without side effects. This is a very experimental library.

### Simple use case

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

### Customization points

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

auto actual = customWriter.tell("bar").log_;
auto expected = MyString("foo\nbar");

REQUIRE(actual == expected);
```
It's also possible to use purely custom Semigroup with a type-erasure technique. For more details see:
`test/test_writer_any_semigroup.cpp`

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
#pragma once

#include <tuple>
#include <iostream>
#include <functional>
#include <memory>
#include <fl/concepts/concepts.hpp>

namespace fl::ops {

namespace details {

template <class F>
struct InvocableAsWriter
{
    using InvocanbleType = std::remove_cvref_t<F>;

    [[nodiscard]] decltype(auto) operator()() { return std::invoke(f); }

    [[nodiscard]] decltype(auto) operator*() { return std::invoke(f); }

    [[nodiscard]] decltype(auto) eval() { return operator()(); }

    InvocanbleType f;
};

template <class F>
InvocableAsWriter(F &&) -> InvocableAsWriter<std::remove_cvref_t<F>>;

template <class T>
concept IsInvokableAsWriter = std::same_as<
    std::remove_cvref_t<T>,
    InvocableAsWriter<typename std::remove_cvref_t<T>::InvocanbleType>
>;

template <class W>
concept SuitableWriter = fl::concepts::IsWriter<W> || details::IsInvokableAsWriter<W>;

template <SuitableWriter WriterLike>
decltype(auto) evaluate(WriterLike &&writerLike)
{
    if constexpr (fl::concepts::IsWriter<WriterLike>) {
        return std::forward<WriterLike>(writerLike);
    } else {
        return std::invoke(std::forward<WriterLike>(writerLike));
    }
}

} // namespace details

struct OperationBase {};

template <class F>
struct Operation : OperationBase {
    using U = std::remove_cvref_t<F>;
    U u;
};

template <class F> struct Tell : Operation<F> {};

template <class F> struct Transform : Operation<F> {};

template <class F> struct AndThen : Operation<F> {};

struct Eval : OperationBase {};

template <class T>
concept BasedOnOperation = std::is_base_of_v<OperationBase, std::remove_cvref_t<T>>;

template <BasedOnOperation T>
using UnderlyingType = typename std::remove_cvref_t<T>::U;

template <class T>
concept IsWriterTellOperation = std::is_same_v<std::remove_cvref_t<T>, Tell<UnderlyingType<T>>>;

template <class T>
concept IsWriterTransformOperation = std::is_same_v<std::remove_cvref_t<T>, Transform<UnderlyingType<T>>>;

template <class T>
concept IsWriterAndThenOperation = std::is_same_v<std::remove_cvref_t<T>, AndThen<UnderlyingType<T>>>;

} // fl::ops

namespace fl {

template <class F>
decltype(auto) transform(F &&f) { return ops::Transform<F>{{{}, std::forward<F>(f)}}; }

template <class F>
decltype(auto) tell(F &&f) { return ops::Tell<F>{{{}, std::forward<F>(f)}}; }

template <class F>
decltype(auto) and_then(F &&f) { return ops::AndThen<F>{{{}, std::forward<F>(f)}};}

static constexpr auto eval = ops::Eval{};

decltype(auto) operator|(fl::ops::details::SuitableWriter auto &&w, ops::IsWriterTellOperation auto &&op)
{
    return fl::ops::details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return fl::ops::details::evaluate(std::move(w)).tell(std::move(op).u);
        }
    };
}

decltype(auto) operator|(fl::ops::details::SuitableWriter auto &&w, ops::IsWriterTransformOperation auto &&op)
{
    return fl::ops::details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return fl::ops::details::evaluate(std::move(w)).transform(std::move(op).u);
        }
    };
}

decltype(auto) operator|(fl::ops::details::SuitableWriter auto &&w, ops::IsWriterAndThenOperation auto &&op)
{
    return fl::ops::details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return fl::ops::details::evaluate(std::move(w)).and_then(std::move(op).u);
        }
    };
}

decltype(auto) operator|(fl::ops::details::IsInvokableAsWriter auto &&w, const ops::Eval &) { return w.eval(); }

} // namespace fl
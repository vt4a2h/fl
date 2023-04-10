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

namespace details {

template <class F>
struct InvocableAsWriter
{
    using writer_invocable_operation_tag_ = std::void_t<>;

    [[nodiscard]] decltype(auto) operator()() { return std::invoke(f); }

    [[nodiscard]] decltype(auto) operator*() { return std::invoke(f); }

    [[nodiscard]] decltype(auto) eval() { return operator()(); }

    std::remove_cvref_t<F> f;
};

template <class F>
InvocableAsWriter(F &&) -> InvocableAsWriter<std::remove_cvref_t<F>>;

template <class T>
concept IsInvokableAsWriter = requires { typename std::remove_cvref_t<T>::writer_invocable_operation_tag_; };

template <class W>
concept SuitableWriter = fl::concepts::IsProbablyWriter<W> || details::IsInvokableAsWriter<W>;

template <class WriterLike>
decltype(auto) evaluate(WriterLike &&writerLike)requires SuitableWriter<WriterLike>
{
    if constexpr (fl::concepts::IsProbablyWriter<WriterLike>) {
        return std::forward<WriterLike>(writerLike);
    } else {
        return std::invoke(std::forward<WriterLike>(writerLike));
    }
}

} // namespace details

namespace fl::ops {

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

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterTellOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).tell(std::move(op).u);
        }
    };
}

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterTransformOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).transform(std::move(op).u);
        }
    };
}

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterAndThenOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).and_then(std::move(op).u);
        }
    };
}

decltype(auto) operator|(details::IsInvokableAsWriter auto &&w, const ops::Eval &) { return w.eval(); }

} // namespace fl
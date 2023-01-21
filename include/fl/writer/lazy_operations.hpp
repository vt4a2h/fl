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

template<class F>
struct InvocableAsWriter
{
    using writer_invocable_operation_tag_ = std::void_t<>;

    [[nodiscard]] decltype(auto) operator()()
    { return std::invoke(f); }
    [[nodiscard]] decltype(auto) operator*()
    { return std::invoke(f); }
    [[nodiscard]] decltype(auto) eval()
    { return operator()(); }

    std::remove_cvref_t<F> f;
};

template<class F>
InvocableAsWriter(F &&) -> InvocableAsWriter<std::remove_cvref_t<F>>;

template<class T>
concept IsInvokableAsWriter = requires { typename std::remove_cvref_t<T>::writer_invocable_operation_tag_; };

template<class W>
concept SuitableWriter = fl::concepts::IsProbablyWriter<W> || details::IsInvokableAsWriter<W>;

template<class WriterLike>
decltype(auto) evaluate(WriterLike &&writerLike)requires SuitableWriter<WriterLike>
{
    if constexpr (fl::concepts::IsProbablyWriter<WriterLike>) {
        return std::forward<WriterLike>(writerLike);
    }
    else {
        return std::invoke(std::forward<WriterLike>(writerLike));
    }
}

} // namespace details

namespace fl::ops {

struct OpDefaultTag {};
struct OpTellTag {};
struct OpTransformTag {};
struct OpAndThenTag {};

struct EvalType {};

template<class F, class OpTag = OpDefaultTag>
struct Op
{
    using tag_ = OpTag;
    std::remove_cvref_t<F> f;
};

template<class T>
concept IsOperation = requires { typename std::remove_cvref_t<T>::tag_; };

template<class T>
concept IsWriterTellOperation =
IsOperation<T> && std::is_same_v<typename std::remove_cvref_t<T>::tag_, OpTellTag>;

template<class T>
concept IsWriterTransformOperation =
IsOperation<T> && std::is_same_v<typename std::remove_cvref_t<T>::tag_, OpTransformTag>;

template<class T>
concept IsWriterAndThenOperation =
IsOperation<T> && std::is_same_v<typename std::remove_cvref_t<T>::tag_, OpAndThenTag>;

} // fl::ops

namespace fl {

template<class F>
decltype(auto) transform(F &&f) { return ops::Op<F, ops::OpTransformTag>{std::forward<F>(f)}; }

template<class F>
decltype(auto) tell(F &&f) { return ops::Op<F, ops::OpTellTag>{std::forward<F>(f)}; }

template<class F>
decltype(auto) and_then(F &&f) { return ops::Op<F, ops::OpAndThenTag>{std::forward<F>(f)}; }

static constexpr auto eval = ops::EvalType{};

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterTellOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).tell(std::move(op).f);
        }
    };
}

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterTransformOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).transform(std::move(op).f);
        }
    };
}

decltype(auto) operator|(details::SuitableWriter auto &&w, ops::IsWriterAndThenOperation auto &&op)
{
    return details::InvocableAsWriter{
        [w = std::forward<decltype(w)>(w), op = std::forward<decltype(op)>(op)]() mutable {
            return details::evaluate(std::move(w)).and_then(std::move(op).f);
        }
    };
}

decltype(auto) operator|(details::IsInvokableAsWriter auto &&w, const ops::EvalType &) { return w.eval(); }

} // namespace fl
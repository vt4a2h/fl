//
// MIT License
//
// Copyright (c) 2022-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#pragma once

#include <fl/concepts/concepts.hpp>

#include <memory>

namespace fl
{

template<class Log>
struct SemigroupWrapper
{
    SemigroupWrapper(SemigroupWrapper &&) = delete;
    SemigroupWrapper& operator=(SemigroupWrapper &&) = delete;

    template<class SemigroupImpl>
    SemigroupWrapper(SemigroupImpl &&semigroupImpl) // NOLINT
    requires (fl::concepts::IsProbablySemigroup<SemigroupImpl> &&
              !std::is_same_v<std::remove_cvref_t<SemigroupImpl>, SemigroupWrapper<Log>>)
        : semigroupModel(std::make_unique<SemigroupModel<SemigroupImpl>>(std::forward<SemigroupImpl>(semigroupImpl)))
    {}

    Log combine(concepts::SameOrConstructable<Log> auto &&l1, concepts::SameOrConstructable<Log> auto &&l2) const
    { return semigroupModel->combineImpl(std::forward<decltype(l1)>(l1), std::forward<decltype(l2)>(l2)); }

    struct SemigroupConcept
    {
        virtual ~SemigroupConcept() = default;
        virtual Log combineImpl(const Log &l1, const Log &l2) const = 0;
        virtual Log combineImpl(const Log &l1, Log &&l2) const = 0;
        virtual Log combineImpl(Log &&l1, const Log &l2) const = 0;
        virtual Log combineImpl(Log &&l1, Log &&l2) const = 0;
    };

    template<fl::concepts::IsProbablySemigroup SemigroupImpl>
    struct SemigroupModel final: SemigroupConcept
    {
        SemigroupModel(concepts::SameOrConstructable<SemigroupImpl> auto &&si) // NOLINT
            : semigroupImpl(std::forward<decltype(si)>(si)) {}

        Log combineImpl(const Log &l1, const Log &l2) const override { return semigroupImpl.combine(l1, l2); }
        Log combineImpl(const Log &l1, Log &&l2) const override { return semigroupImpl.combine(l1, std::move(l2)); }
        Log combineImpl(Log &&l1, const Log &l2) const override { return semigroupImpl.combine(std::move(l1), l2); }
        Log combineImpl(Log &&l1, Log &&l2) const override { return semigroupImpl.combine(std::move(l1), std::move(l2)); }

        SemigroupImpl semigroupImpl;
    };

    std::unique_ptr<SemigroupConcept> semigroupModel;
};

} // namespace fl
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
    template<fl::concepts::IsProbablySemigroup T>
    explicit SemigroupWrapper(T &&semigroupImpl)
        : semigroupModel(std::make_unique<SemigroupModel<decltype(semigroupImpl)>>(
            std::forward<decltype(semigroupImpl)>(semigroupImpl)))
    {}

    Log combine(const Log &l1, const Log &l2) const { return semigroupModel->combineImpl(l1, l2); }
    Log combine(const Log &l1, Log &&l2) const { return semigroupModel->combineImpl(l1, std::move(l2)); }
    Log combine(Log &&l1, const Log &l2) const { return semigroupModel->combineImpl(std::move(l1), l2); }
    Log combine(Log &&l1, Log &&l2) const { return semigroupModel->combineImpl(std::move(l1), std::move(l2)); }

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
        explicit SemigroupModel(const SemigroupImpl& si) : semigroupImpl(si) {}
        explicit SemigroupModel(SemigroupImpl&& si) : semigroupImpl(std::move(si)) {}

        Log combineImpl(const Log &l1, const Log &l2) const override { return semigroupImpl.combine(l1, l2); }
        Log combineImpl(const Log &l1, Log &&l2) const override { return semigroupImpl.combine(l1, std::move(l2)); }
        Log combineImpl(Log &&l1, const Log &l2) const override { return semigroupImpl.combine(std::move(l1), l2); }
        Log combineImpl(Log &&l1, Log &&l2) const override { return semigroupImpl.combine(std::move(l1), std::move(l2)); }

        SemigroupImpl semigroupImpl;
    };

    std::unique_ptr<SemigroupConcept> semigroupModel;
};

} // namespace fl
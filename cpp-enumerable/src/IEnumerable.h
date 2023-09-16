#pragma once

#include "IEnumerator.h"

namespace cppe {

    template<typename T>
    class IEnumerable
    {
    public:
        static constexpr bool IsObjectType = std::is_object_v<T>;

        using ReturnType = std::conditional_t<IsObjectType, T&, T>;

    public:
        virtual IEnumerator<T&> GetEnumerator() { co_return; };
        virtual IEnumerator<const T&> GetEnumerator() const { co_return; }

    public:
		template<typename Func> requires IsFunc<Func, const T&, const T&, const T&>
        T Aggregate(Func&& func) const
        {
            T result = T();
            for (const T& next : GetEnumerator())
            {
                result = func(result, next);
            }
            return result;
        }
		template<typename Func, typename TAccumulate> requires IsFunc<Func, const TAccumulate&, const TAccumulate&, const T&>
        TAccumulate Aggregate(const TAccumulate& seed, Func&& func) const
        {
            TAccumulate result = seed;
            for (const T& next : GetEnumerator())
            {
                result = func(result, next);
            }
            return result;
        }
		template<typename FuncAggr, typename FuncSelect, typename TAccumulate, typename TResult> requires 
            IsFunc<FuncAggr, const TAccumulate&, const TAccumulate&, const T&> &&
            IsFunc<FuncSelect, const TResult&, const TAccumulate&>
        TResult Aggregate(const TAccumulate& seed, FuncAggr&& func, FuncSelect&& resultSelector) const
        {
            return resultSelector(Aggregate<FuncAggr, TAccumulate>(seed, std::move(func)));
        }

		template<typename Func> requires IsFunc<Func, bool, const T&>
        bool All(Func&& predicate) const
        {
            for (const T& next : GetEnumerator())
            {
                if (!predicate(next))
                    return false;
            }

            return true;
        }
		template<typename Func> requires IsFunc<Func, bool, const T&>
        bool Any(Func&& predicate) const
        {
            for (const T& next : GetEnumerator())
            {
                if (predicate(next))
                    return true;
            }

            return false;
        }

        auto Append(const T& val) const
        {
            struct AppendEnumerable : IEnumerable<T>
            {
                const IEnumerable<T>* context;
                const T& value;

                AppendEnumerable(const IEnumerable<T>* ctx, const T& val)
                    : context(ctx), value(val) {}

                IEnumerator<const T&> GetEnumerator() const override
                {
                    for (auto seqVal : context->GetEnumerator())
                    {
                        co_yield seqVal;
                    }

                    co_yield value;
                }
            };

            return AppendEnumerable(this, val);
        }

        const IEnumerable<T>& AsEnumerable() const { return *this; }

        auto Concat(const IEnumerable<T>& second) const
        {
            struct ConcatEnumerable : IEnumerable<T>
            {
                const IEnumerable<T>* context;
                const IEnumerable<T>& second;

                ConcatEnumerable(const IEnumerable<T>* ctx, const IEnumerable<T>& next)
                    : context(ctx), second(next) {}

                IEnumerator<const T&> GetEnumerator() const override
                {
                    for (auto seqVal : context->GetEnumerator())
                    {
                        co_yield seqVal;
                    }
                    for (auto seqVal : second.GetEnumerator())
                    {
                        co_yield seqVal;
                    }

                }
            };

            return ConcatEnumerable(this, second);
        }
    };
}
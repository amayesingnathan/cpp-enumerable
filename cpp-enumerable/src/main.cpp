#include "pch.h"
#include "IEnumerable.h"

using namespace cppe;

struct FooType : public IEnumerable<int>
{
    std::vector<int> a = { 1, 6, 3, 4, 86 };

    IEnumerator<const int&> GetEnumerator() const override
    {
        for (int i : a)
        {
            co_yield i;
        }
    }
};

#define foreach(expr) for (expr.GetEnumerator())

int main()
{
    FooType a;
    //std::cout << a.Aggregate([] (int accumulate, int next) { return accumulate += next; });

    int b = 10;
    const IEnumerable<int>& c = a.Append(b);

    foreach (int i : c)
    {
        std::cout << i << "\n";
    }

    foreach (int i : a.Concat(a))
    {
        std::cout << i << "\n";
    }

    std::cout << std::endl;
}
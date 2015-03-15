/*
 @ Kingsley Chen
*/

#include <conio.h>

#include <iostream>

#include "yielderator.h"

class Collection {
public:
    using value_type = int;

    void Yielderate()
    {
        for (int i = 1; i <= 10; ++i) {
            if (i > 5) {
                yield_break;
            }

            yield_return(i);
        }
    }
};

int main()
{
    Collection c;
    Yielderator<Collection> yd(&c);
    Yielderator<Collection> partial_yd(&c);

    std::cout << "partial: ";
    partial_yd.MoveNext();
    std::cout << *partial_yd.Current() << " ";
    partial_yd.MoveNext();
    std::cout << *partial_yd.Current() << std::endl;

    std::cout << "complete: ";
    while (yd.MoveNext()) {
        std::cout << *yd.Current() << " ";
    }
    std::cout << std::endl;

    partial_yd.MoveNext();
    std::cout << "partial again: " << *partial_yd.Current();

    _getch();
    return 0;
}
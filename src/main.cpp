/*
 @ Kingsley Chen
*/

#include <conio.h>

#include <iostream>
#include <string>

#include "yielderator.h"

class Collection {
public:
    using value_type = std::string;

private:
    void Yielderate()
    {
        yield_return(std::string("abc"));
        yield_return(std::string("def"));
        yield_return(std::string("hij"));
        yield_return(std::string("hello"));
        yield_return(std::string("world"));
    }

    friend Yielderator<Collection>;
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
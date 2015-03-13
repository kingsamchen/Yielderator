/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef YIELDERATOR_H_
#define YIELDERATOR_H_

// An Yielderator object is neither copyable nor movable.
// `value_type` of elements in `Container` must support default construction and
// copy-semantics.
template<typename Container>
class Yielderator {
private:
    using Fiber = void*;

public:
    using value_type = typename Container::value_type;

    Yielderator();

    ~Yielderator();

    Yielderator(const Yielderator&) = delete;

    Yielderator(Yielderator&&) = delete;

    Yielderator& operator=(const Yielderator&) = delete;

    Yielderator& operator=(Yielderator&&) = delete;

    bool MoveNext();

    value_type& Current();

    const value_type& Current() const;

private:
    value_type current_value_;
    Fiber self_ = nullptr;
    Fiber iterator_ = nullptr;
};

#endif  // YIELDERATOR_H_
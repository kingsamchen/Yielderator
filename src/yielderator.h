/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef YIELDERATOR_H_
#define YIELDERATOR_H_

#include <Windows.h>

// An Yielderator object is neither copyable nor movable.
// `value_type` of elements in `Container` must support copy-semantics.
// As a matter of fact, handling exceptions in the `Yielderator` can be tricky, so
// I haven't get them properly handled yet. For the time being, the `Yielderator`
// can be regarded as no-throw.
template<typename Container>
class Yielderator {
public:
    using value_type = typename Container::value_type;

    Yielderator(Container* source)
        : source_(source)
    {}

    ~Yielderator()
    {
        // The other Yielderator objects may be still counting on the current
        // fiber-ized thread. We cannot just convert it back.
        DeleteFiber(iterator_);
        iterator_ = nullptr;
        self_ = nullptr;
    }

    Yielderator(const Yielderator&) = delete;

    Yielderator(Yielderator&&) = delete;

    Yielderator& operator=(const Yielderator&) = delete;

    Yielderator& operator=(Yielderator&&) = delete;

    // Returns true, if we advanced to the next element.
    // Returns false, if we are done with the iteration.
    // The function must run in self-fiber.
    bool MoveNext()
    {
        // Already done with iteration.
        if (!source_) {
            return false;
        }

        // Do some preparations if we are new to this.
        if (!self_) {
            current_value_ =
                static_cast<value_type*>(operator new(sizeof(value_type)));
            SetupFiber();
        }

        SwitchToFiber(iterator_);

        // Are we done with iteration?
        if (!source_) {
            return false;
        }

        return true;
    }

    value_type* Current()
    {
        return current_value_;
    }

    const value_type* Current() const
    {
        return current_value_;
    }

private:
    using Fiber = void*;

    class ValueProxy {
    public:
        ValueProxy() = default;

        ~ValueProxy()
        {
            if (value_) {
                value_->~value_type();
                operator delete(value_);
                value_ = nullptr;
            }
        }

        ValueProxy(const ValueProxy&) = delete;

        ValueProxy(ValueProxy&&) = delete;

        ValueProxy& operator=(const ValueProxy&) = delete;

        ValueProxy& operator=(ValueProxy&&) = delete;

        ValueProxy& operator=(const value_type& val)
        {
            if (!value_) {
                value_ = static_cast<value_type*>(operator new(sizeof(value_type)));
                new (value_) value_type(val);
            } else {
                *value_ = val;
            }

            return *this;
        }

    private:
        value_type* value_ = nullptr;
    };

    void SetupFiber()
    {
        // The other Yielderator objects may have made current thread a fiber.
        self_ = IsThreadAFiber() ?
                    GetCurrentFiber() :
                    ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);

        iterator_ = CreateFiberEx(0, 4096, FIBER_FLAG_FLOAT_SWITCH, IteratorProc,
                                  this);
    }

    // With no doubt, this function runs in iterator-fiber, and so does the
    // `Yielderate` function.
    static void CALLBACK IteratorProc(void* param)
    {
        Yielderator* yielderator = static_cast<Yielderator*>(param);
        Fiber host = yielderator->self_;

        yielderator->source_->Yielderate();

        yielderator->source_ = nullptr;
        SwitchToFiber(host);
    }

    template<typename ResultType>
    friend void yield_return(const ResultType& result);

private:
    Container* source_;
    value_type* current_value_ = nullptr;
    Fiber self_ = nullptr;
    Fiber iterator_ = nullptr;
};

// This function is invoked within the call of `Yielderate`. So it runs in
// iterator-fiber.
template<typename ResultType>
void yield_return(const ResultType& result)
{
    // Since all members of a Yielderator object are of fixed size, we can access
    // them without knowing what exact type the Yielderator object is.
    // However, this "convenience" may also incur data corruption if `ResultType`
    // doesn't match `current_value_`'s type.
    struct Dummy {
        using value_type = ResultType;
    };

    auto yielderator = static_cast<Yielderator<Dummy>*>(GetFiberData());
    *yielderator->current_value_ = result;

    SwitchToFiber(yielderator->self_);
}

#define yield_break { return; }

#endif  // YIELDERATOR_H_
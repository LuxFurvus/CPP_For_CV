#pragma once

#include <type_traits>
#include <memory>
#include <utility>

template <typename Signature>
class Delegate;

// Specialization for R(Args...)
template <typename R, typename... Args>
class Delegate<R(Args...)>
{
private:
    using StubType = R (*)(void*, Args&&...);

    void* Object = nullptr;
    StubType Stub = nullptr;

    std::unique_ptr<void, void(*)(void*)> Storage { nullptr, nullptr };

private:
    // -------- Free function --------
    template <R (*Function)(Args...)>
    static R FunctionStub(void*, Args&&... args)
    {
        return Function(std::forward<Args>(args)...);
    }

    // -------- Method --------
    template <class T, R (T::*Method)(Args...)>
    static R MethodStub(void* obj, Args&&... args)
    {
        return (static_cast<T*>(obj)->*Method)(
            std::forward<Args>(args)...);
    }

    // -------- Const method --------
    template <class T, R (T::*Method)(Args...) const>
    static R ConstMethodStub(void* obj, Args&&... args)
    {
        return (static_cast<const T*>(obj)->*Method)(
            std::forward<Args>(args)...);
    }

    // -------- Lambda / functor --------
    template <class Callable>
    static R LambdaStub(void* obj, Args&&... args)
    {
        return (*static_cast<Callable*>(obj))(
            std::forward<Args>(args)...);
    }

public:
    Delegate() = default;

    // -------- Free function --------
    template <R (*Function)(Args...)>
    static Delegate FromFunction()
    {
        Delegate D;
        D.Object = nullptr;
        D.Stub = &FunctionStub<Function>;
        return D;
    }

    // -------- Method --------
    template <class T, R (T::*Method)(Args...)>
    static Delegate FromMethod(T* Instance)
    {
        Delegate D;
        D.Object = Instance;
        D.Stub = &MethodStub<T, Method>;
        return D;
    }

    // -------- Const method --------
    template <class T, R (T::*Method)(Args...) const>
    static Delegate FromConstMethod(const T* Instance)
    {
        Delegate D;
        D.Object = const_cast<T*>(Instance);
        D.Stub = &ConstMethodStub<T, Method>;
        return D;
    }

    // -------- Lambda / functor --------
    template <class Callable>
    static Delegate FromLambda(Callable&& CallableObj)
    {
        using StoredType = std::decay_t<Callable>;

        auto* HeapCallable = new StoredType(
            std::forward<Callable>(CallableObj));

        Delegate D;
        D.Object = HeapCallable;
        D.Stub   = &LambdaStub<StoredType>;
        D.Storage = {
            HeapCallable,
            [](void* Ptr)
            {
                delete static_cast<StoredType*>(Ptr);
            }
        };

        return D;
    }

    // -------- Invocation --------
    R operator()(Args... args) const
    {
        if constexpr (std::is_void_v<R>)
        {
            if (Stub)
            {
                Stub(Object, std::forward<Args>(args)...);
            }
        }
        else
        {
            return Stub
                ? Stub(Object, std::forward<Args>(args)...)
                : R{};
        }
    }

    explicit operator bool() const
    {
        return Stub != nullptr;
    }
};

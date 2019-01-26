#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <memory>
#include <functional>

constexpr size_t SMALL_SIZE = 32;

template <typename T>
class function;

template <typename ReturnType, typename... Args>
class function<ReturnType(Args...)>
{
public:
    function() noexcept : small(false), bigStorage() {}
    function(std::nullptr_t) noexcept : function() {}

    function(function const& other): small(other.small)
    {
        if (small)
            (reinterpret_cast<function_storage_base const*>(other.smallStorage))->cloneTo(smallStorage);
        else
            bigStorage = other.bigStorage->clone();
    }

    function(function&& other) noexcept: small(other.small)
    {
        if (small)
        {
            (reinterpret_cast<function_storage_base*>(other.smallStorage))->moveTo(smallStorage);
        }
        else
        {
            bigStorage = std::move(other.bigStorage);
        }
    }

    template <typename CallableType>
    function(CallableType f)
    {
        if constexpr (sizeof(function_storage<CallableType>) <= SMALL_SIZE * sizeof(char))
        {
            small = true;
            new (&smallStorage) function_storage<CallableType>(std::move(f));
        }
        else
        {
            small = false;
            bigStorage = std::make_unique<function_storage<CallableType>>(f);
        }
    }

    ~function()
    {
        if (small)
            reinterpret_cast<function_storage_base*>(smallStorage)->~function_storage_base();
        else
            bigStorage.reset();
    }

    void swap(function& other) noexcept
    {
        std::swap(small, other.small);
        char tmp[SMALL_SIZE];
        memcpy(tmp, other.smallStorage, SMALL_SIZE);
        memcpy(other.smallStorage, smallStorage, SMALL_SIZE);
        memcpy(smallStorage, tmp, SMALL_SIZE);
    }

    function& operator=(function const& other)
    {
        auto tmp(other);
        this->swap(tmp);
        return *this;
    }

    function& operator=(function&& other) noexcept
    {
        this->swap(other);
        return *this;
    }

    ReturnType operator()(Args&&... args) const
    {
        if (small)
            return (reinterpret_cast<function_storage_base*>(smallStorage))->invoke(std::forward<Args>(args)...);
        else if (bigStorage)
             return bigStorage->invoke(std::forward<Args>(args)...);
        else
            throw std::bad_function_call();
    }

    explicit operator bool() const noexcept
    {
        return small || bigStorage;
    }

private:
    class function_storage_base
    {
    public:
        function_storage_base() noexcept {}
        virtual ~function_storage_base() noexcept {}
        virtual ReturnType invoke(Args&&... args) = 0;
        virtual std::unique_ptr<function_storage_base> clone() const noexcept = 0;
        virtual void cloneTo(void* destination) const = 0;
        virtual void moveTo(void* destination) = 0;

        function_storage_base(function_storage_base const&) = delete;
        void operator= (function_storage_base const&) = delete;
    };

    template <typename CallableType>
    class function_storage : public function_storage_base
    {
        CallableType func;

    public:
        function_storage() noexcept: function_storage_base() {}
        function_storage(CallableType const& f) noexcept: function_storage_base(), func(f) {}
        function_storage(CallableType&& f) noexcept : func(std::move(f)) {}

        ReturnType invoke(Args&&... args)
        {
            return func(args...);
        }

        std::unique_ptr<function_storage_base> clone() const noexcept
        {
            return std::make_unique<function_storage>(func);
        }

        void cloneTo(void* destination) const
        {
            new (destination) function_storage<CallableType>(func);
        }

        virtual void moveTo(void* destination)
        {
            new (destination) function_storage<CallableType>(std::move(func));
        }
    };

private:
    bool small;
    union
    {
        mutable char smallStorage[SMALL_SIZE];
        std::unique_ptr<function_storage_base> bigStorage;
    };
};

#endif //FUNCTION_FUNCTION_H

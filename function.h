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
            memcpy(smallStorage, other.smallStorage, SMALL_SIZE);
        else
            bigStorage = other.bigStorage->clone();
    }

    function(function&& other) noexcept: small(other.small)
    {
        memmove(smallStorage, other.smallStorage, SMALL_SIZE);
    }

    template <typename CallableType>
    function(CallableType f)
    {
        if constexpr (sizeof(function_storage<CallableType>) <= SMALL_SIZE * sizeof(char))
        {
            small = true;
            new (&smallStorage) function_storage<CallableType>(f);
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
        small = other.small;
        if (small)
            memcpy(smallStorage, other.smallStorage, SMALL_SIZE);
        else
            bigStorage = other.bigStorage->clone();
        return *this;
    }

    function& operator=(function&& other) noexcept
    {
        this->swap(other);
        return *this;
    }

    ReturnType operator()(Args... args)
    {
        if (small)
            return (reinterpret_cast<function_storage_base*>(smallStorage))->invoke(args...);
        else if (bigStorage)
             return bigStorage->invoke(args...);
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
        virtual ReturnType invoke(Args... args) = 0;
        virtual std::unique_ptr<function_storage_base> clone() noexcept = 0;

        function_storage_base(function_storage_base const&) = delete;
        void operator= (function_storage_base const&) = delete;
    };

    template <typename CallableType>
    class function_storage : public function_storage_base
    {
        CallableType func;

    public:
        function_storage(CallableType f) noexcept: function_storage_base(), func(f) {}
        ReturnType invoke(Args... args)
        {
            return func(args...);
        }

        std::unique_ptr<function_storage_base> clone() noexcept
        {
            return std::make_unique<function_storage>(func);
        }
    };

private:
    bool small;
    union
    {
        char smallStorage[SMALL_SIZE];
        std::unique_ptr<function_storage_base> bigStorage;
    };
};

#endif //FUNCTION_FUNCTION_H

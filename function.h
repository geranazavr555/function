#ifndef FUNCTION_FUNCTION_H
#define FUNCTION_FUNCTION_H

#include <memory>

template <typename T>
class function;

template <typename ReturnType, typename... Args>
class function<ReturnType(Args...)>
{
public:
    function() noexcept : storage() {}
    function(std::nullptr_t) noexcept : storage() {}
    function(function const& other): storage(other.storage->clone()) {}

    template <typename CallableType>
    function(CallableType f): storage(new function_storage<CallableType>(f)) {}

    function& operator=(function const& other)
    {
        storage = other.storage->clone();
        return *this;
    }

    ReturnType operator()(Args... args)
    {
        return storage->invoke(args...);
    }



private:
    class function_storage_base
    {
    public:
        function_storage_base() {}
        virtual ~function_storage_base() {}
        virtual ReturnType invoke(Args... args) = 0;
        virtual std::unique_ptr<function_storage_base> clone() = 0;

        function_storage_base(function_storage_base const&) = delete;
        void operator= (function_storage_base const&) = delete;
    };

    typedef std::unique_ptr<function_storage_base> StorageType;

    template <typename CallableType>
    class function_storage : public function_storage_base
    {
        CallableType func;

    public:
        function_storage(CallableType f): function_storage_base(), func(f) {}
        ReturnType invoke(Args... args)
        {
            return func(args...);
        }

        StorageType clone()
        {
            return StorageType(new function_storage<CallableType>(func));
        }

    };

private:
    StorageType storage;
};

#endif //FUNCTION_FUNCTION_H

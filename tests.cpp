#include <gtest/gtest.h>
#include <function.h>
#include <functional>

void void_none_args_func()
{
    int t = 2 + 2;
}

int two_plus_two()
{
    int t = 2 + 2;
    return t;
}

int sum(int a, int b)
{
    return a + b;
}

TEST(constuctors, default_)
{
    try
    {
        function<void()> f;
        SUCCEED();
    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
        FAIL();
    }
}

TEST(constuctors, nullptr_t)
{
    try
    {
        function<void()> f(nullptr_t);
        SUCCEED();
    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
        FAIL();
    }
}

TEST(constuctors, function)
{
    try
    {
        function<void()> f(void_none_args_func);
        SUCCEED();
    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
        FAIL();
    }
}

TEST(constuctors, bind)
{
    try
    {
        function<int(int, int)> f(std::bind(sum, 2, 2));
        SUCCEED();
    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
        FAIL();
    }
}

TEST(constuctors, lambda)
{
    try
    {
        function<int(int, int)> f([](int a, int b){return 4;});
        SUCCEED();
    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
        FAIL();
    }
}


TEST(invokation, simple)
{
    function<int()> f(two_plus_two);
    ASSERT_EQ(f(), 4);
}

TEST(invokation, arguments_simple)
{
    function<int(int, int)> f(sum);
    ASSERT_EQ(f(2, 2), 4);
}

TEST(copying, constructor)
{
    function<int()> f(two_plus_two);
    function<int()> g(f);
    ASSERT_EQ(g(), 4);
}

TEST(copying, operator_eq)
{
    function<int()> f(two_plus_two);
    function<int()> g(std::bind(sum, 3, 3));
    ASSERT_EQ(g(), 6);
    g = f;
    ASSERT_EQ(g(), 4);
}

TEST(copying, maxim_test)
{
    std::vector<int> a = {1, 2, 3};
    function<int()> f([a](){return a[1];});
    function<int()> g(f);
    ASSERT_EQ(f(), 2);
    ASSERT_EQ(g(), 2);
}

TEST(swap, simple)
{
    function<int()> f([](){return 5;});
    function<int()> g([](){return 6;});
    ASSERT_EQ(f(), 5);
    ASSERT_EQ(g(), 6);
    std::swap(f, g);
    ASSERT_EQ(f(), 6);
    ASSERT_EQ(g(), 5);
}

TEST(moving, simple)
{
    function<int()> f([](){return 5;});
    function<int()> g = std::move(f);
    ASSERT_EQ(g(), 5);
}

TEST(moving, operator_eq)
{

    function<int()> f([](){return 5;});
    function<int()> g([](){return 6;});
    ASSERT_EQ(g(), 6);
    g = std::move(f);
    ASSERT_EQ(g(), 5);
}

TEST(bool_check, negative)
{
    function<int()> f;
    bool c = (f ? true : false);
    ASSERT_FALSE(c);
}

TEST(bool_check, positive)
{
    function<int()> f(two_plus_two);
    bool c = (f ? true : false);
    ASSERT_TRUE(c);
}

TEST(other, main)
{
    struct X
    {
        X()
        {
            std::cout << "main\n";
        }

        X(X const& other)
        {
            std::cout << "copy\n";
        }

        X(X&& other)
        {
            std::cout << "move\n";
        }

        void operator()()
        {
            std::cout << "call\n";
        }
    };

    function<void()> f = X();
    std::function<void()> g = X();
}
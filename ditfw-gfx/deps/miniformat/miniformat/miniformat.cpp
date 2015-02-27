// miniformat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "miniformat.h"
#include <string>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>

template <typename T>
void test()
{
    T str;
    mini::format(str, "String: %1 Int: %0, Float: %(.3)2\n", 100, "JJ", 3.141592);
    assert(std::string(str) == "String: JJ Int: 100, Float: 3.142\n");
    mini::format(str, "%1 %1 %1\n", 100, "JJ", 3.141592);
    assert(std::string(str) == "JJ JJ JJ\n");
    mini::format(str, "%(.2)2 %(.3)2 %(.4)2\n", 100, "JJ", 3.141592);
    assert(std::string(str) == "3.14 3.142 3.1416\n");
    mini::format(str, "%2 %1 %0 %0 %1 %2\n", 100, "JJ", 3.141592);
    assert(std::string(str) == "3.141592 JJ 100 100 JJ 3.141592\n");
    mini::format(str, "%0\n", "P1");
    assert(std::string(str) == "P1\n");
    mini::format(str, "%0\n", 7);
    assert(std::string(str) == "7\n");
    mini::format(str, "%0\n", 3.14);
    assert(std::string(str) == "3.140000\n");
    mini::format(str, "%0 %1\n", "one", "two");
    assert(std::string(str) == "one two\n");
    mini::format(str, "%0 %1 %2 %3\n", "one", "two", "three", "four");
    assert(std::string(str) == "one two three four\n");
    mini::format(str, "%0 %1 %2 %3 %4\n", "one", "two", "three", "four", "five");
    assert(std::string(str) == "one two three four five\n");
    mini::format(str, "%0 %1 %2 %3 %4 %5\n", "one", "two", "three", "four", "five", "six");
    assert(std::string(str) == "one two three four five six\n");
    mini::format(str, "%0 %1 %2 %3 %4 %5 %6\n", "one", "two", "three", "four", "five", "six", "seven");
    assert(std::string(str) == "one two three four five six seven\n");
    mini::format(str, "%0 %1 %2 %3 %4 %5 %6 %7\n", "one", "two", "three", "four", "five", "six", "seven", "eight");
    assert(std::string(str) == "one two three four five six seven eight\n");
    mini::format(str, "%0 %1 %2 %3 %4 %5 %6 %7 %8\n", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine");
    assert(std::string(str) == "one two three four five six seven eight nine\n");
    mini::format(str, "%0 %%, %%0\n", "Literal");
    assert(std::string(str) == "Literal %, %0\n");
    mini::format(str, "%(6)0\n", 100);
    assert(std::string(str) == "   100\n");
    mini::format(str, "%(2)0\n", 100);
    assert(std::string(str) == "100\n");
    mini::format(str, "%(3)0\n", 100);
    assert(std::string(str) == "100\n");
    mini::format(str, "%(6.2)0\n", 3.14);
    assert(std::string(str) == "  3.14\n");
    mini::format(str, "%(6.2)0\n", -3.14);
    assert(std::string(str) == " -3.14\n");
    mini::format(str, "%0%1%2", 'J', 'J', '1');
    assert(std::string(str) == "JJ1");
    // Followings should assert in debug.
    //mini::format(std::string(str), "%0 %n\n", 3);
    //mini::format(std::string(str), "%(.3)1\n", 3.141592);
    //mini::format(std::string(str), "String: %1 Int: %0, Float: %(.3)3\n", 100, "JJ", 3.141592);
}

void benchmark()
{
    const int count = 1000000;
    std::chrono::steady_clock::time_point c0, c1;
    double dt0, dt1;

    std::cout << std::endl;

    std::cout << "sprintf_s" << std::endl;
    char buf[100];
    c0 = std::chrono::steady_clock::now();
    for(int i=0; i<count; ++i)
    {
        sprintf_s(buf, 100, "P1:%d P2:%f P3:%s", i, (double)i, "test");
    }
    c1 = std::chrono::steady_clock::now();
    dt0 = std::chrono::duration<double, std::ratio<1, 1>>(c1 - c0).count(); 
    std::cout << dt0 << "seconds" << std::endl;

    std::cout << std::endl;

    std::cout << "mini::format(to std::string)" << std::endl;
    std::string res;
    res.reserve(100);
    c0 = std::chrono::steady_clock::now();
    for(int i=0; i<count; ++i)
    {
        mini::format(res, "P1:%0 P2:%1 P3:%2", i, (double)i, "test");
    }
    c1 = std::chrono::steady_clock::now();
    dt1 = std::chrono::duration<double, std::ratio<1, 1>>(c1 - c0).count(); 
    std::cout << dt1 << "seconds" << std::endl;

    std::cout << std::endl << "Speed-up: " << dt0/dt1 << std::endl;

    std::cout << std::endl;

    std::cout << "mini::format(to char array)" << std::endl;
    char res2[100];
    c0 = std::chrono::steady_clock::now();
    for(int i=0; i<count; ++i)
    {
        mini::format(res2, "P1:%0 P2:%1 P3:%2", i, (double)i, "test");
    }
    c1 = std::chrono::steady_clock::now();
    dt1 = std::chrono::duration<double, std::ratio<1, 1>>(c1 - c0).count(); 
    std::cout << dt1 << "seconds" << std::endl;

    std::cout << std::endl << "Speed-up: " << dt0/dt1 << std::endl;

    std::cout << std::endl;
}

template <typename T>
void benchmark2()
{
    const int dataCount = 1000000;

    static std::string output;
    output.reserve(32);
    std::vector<T> data;
    data.resize(dataCount, 0);
    for(int i=0; i<dataCount; ++i)
        data[i] = i;
    std::random_shuffle(data.begin(), data.end());
    std::chrono::steady_clock::time_point c0, c1;
    c0 = std::chrono::steady_clock::now();
    for(int i=0; i<dataCount; ++i)
        mini::format(output, "%0", data[i]);
    c1 = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double, std::ratio<1, 1>>(c1 - c0).count(); 
    std::cout << dt << "seconds" << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
    test<std::string>();
    test<char[128]>();

    benchmark();
    //benchmark2<uint32_t>();
    //benchmark2<int32_t>();
    //benchmark2<uint64_t>();
    //benchmark2<int64_t>();

    getchar();

    return 0;
}

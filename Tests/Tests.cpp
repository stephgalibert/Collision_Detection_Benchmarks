#include <iostream>

struct Test
{
    int a;
    int b;
    int c;
    int d;
};

int main()
{
    Test test;
    test.a + 2;
    unsigned* a = reinterpret_cast<unsigned*>(&test);
    
    std::cout << "Hello World!\n";
    std::cout << test.a << std::endl;
    // a[0] = 12;
    reinterpret_cast<unsigned*>(&test)[0] = 13;
    std::cout << test.a << std::endl;
    return 0;
}
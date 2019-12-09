#include <cstdio>
#include <string>
#include <iostream>
#include <cctype>

class Woo {
    public:
        virtual ~Woo() {}
};
class Nim : public Woo {
    double Val;
public:
    Nim(double Val) : Val(Val) {}
};

int main(int argc, char const *argv[])
{
    Nim::Val test;
    return 0;
}

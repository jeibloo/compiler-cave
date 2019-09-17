#include <cstdio>
#include <string>
#include <iostream>
#include <cctype>

int main(int argc, char const *argv[])
{
    int thing = ' ';
    while(isspace(thing)) {
        thing = getchar();
    }
    /*
    do
        thing = getchar();
    while (thing != EOF && thing != '\n' && thing != '\r');
    */
    std::cout << thing << std::endl;
    return 0;
}

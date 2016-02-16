#include <cstdio>
#include <miniconf.h>

int main(int argc, char** argv)
{
    int x = 11;
    int y = 12;
    Value v1(x);
    Value v2(123.23);
    Value v4((Value)Value(v1));
    Value v5(false);
    Value v6("asdfadf");
    Value v7(123.23);
    v7 = "adfdafdfaf";

    printf("%d\n", v1.getInt());
    printf("%f\n", v2.getNumber());
    printf("%d\n", v4.getInt());
    printf("%d\n", v5.getBoolean());
    printf("%s\n", v6.getCharArray());
    printf("%s\n", v7.getCharArray());
    printf("%s\n", v7.getString().c_str());

    return 0;
}

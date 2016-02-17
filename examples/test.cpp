#include <cstdio>
#include <miniconf.h>

int main(int argc, char** argv)
{

    Config conf;
    
    printf("Start\n");

    conf.option("abc").defaultValue(3).shortflag("v").required(true);
    conf.option("abcc").defaultValue("diu").shortflag("e").required(false);
    conf.printOptions();



    conf.parse(argc, argv);
    conf.log();
    
    conf.print();

    return 0;
}

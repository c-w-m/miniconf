#include <cstdio>
#include <miniconf.h>

int main(int argc, char** argv)
{
    Config conf;
    conf.description("Miniconfig test program");
    conf.option("int").shortflag("i").defaultValue(3).required(true).description("int value");
    conf.option("number").shortflag("n").defaultValue(3.14).required(true).description("number value");
    conf.option("bool").shortflag("b").defaultValue(false).required(true).description("boolean value");
    conf.option("string").shortflag("s").defaultValue("this is a string").required(true).description("string value");
    conf.option("string2").shortflag("s2").defaultValue("this is a string").required(true).description("string value");
    conf.option("string3").shortflag("s3").defaultValue("this is a string").required(true).description("string value");
    conf.option("string4").shortflag("s4").defaultValue("this is a string").required(true).description("string value");
    conf.option("string5").shortflag("s5").defaultValue("this is a string").required(true).description("string value");
    conf.log(Config::LogLevel::INFO);
    conf.parse(argc, argv);
    conf.print();
    conf.serialize();
    return 0;
}

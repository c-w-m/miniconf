#include <cstdio>
#include <miniconf.h>

/* Main file */
int main(int argc, char** argv)
{
    // create a Config object
    miniconf::Config conf;

    // Set up program description
    conf.description("A simple example for miniconf");

    // Setup config options 
    // Option properties, e.g. short flag
    conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
    conf.option("intOpt").shortflag("d").defaultValue(122).required(false).description("A integer value");
    conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
    conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");

    // parse 
    conf.parse(argc, argv);

    // you may print the values out...
    conf.print();

    return 0;
}

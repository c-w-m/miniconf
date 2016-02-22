#include <cstdio>
#include <miniconf.h>

int main(int argc, char** argv)
{
   // create a Config object
   miniconf::Config conf;

   // Setup config options 
   // Option properties, e.g. short flag
   conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
   conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
   conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");

   // parse 
   conf.parse(argc, argv);

   conf.print();

   // processing the configuration here ...

   return 0;
}

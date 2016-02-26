#include <cstdio>
#include <miniconf.h>


/* 
 * this is a test function 
 */
int testFunction(int x)
{
    x = x * 2;
    return x;
}

/* 
 * class x for internal testing *
 */
class X
{
    public: 
        X(){
            x = 10;
            y = 12;
        }
        int x;
        int y;
};

/* Main file
 *
 * Hello world
 */
int main(int argc, char** argv)
{
   // create a Config object
   miniconf::Config conf;

    X xx;

   // Setup config options 
   // Option properties, e.g. short flag
   conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
   conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
   conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");
   conf.option("str.hello.diu.Opt2").shortflag("q").defaultValue("string").required(true).description("A string value");

   // parse 
   conf.parse(argc, argv);

   conf.print();

   conf.serialize("xxx.json");

   // processing the configuration here ...

   return 0;
}

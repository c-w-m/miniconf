
## miniconf.h

A minimalist C++ configuration manager

## Features

   This is a simple manager to read, store and serialize application
   configurations of a C++ program. The main features are:

    * Define configuration format easily
    * Parse configuration from command line arguments
    * Format checking, user input validation, default values, etc.
    * Generate help and usage message automatically
    * (TODO) support simple JSON and CVS serialization
    
## User Manual

### Setting up a configuration for an application

```c++
int main(int argc, char** argv)
{
   // create a Config object
   thyu::Config conf;
   
   // Setup config options 
   conf.option("intOpt").shortflag("i").defaultValue(3).required(false).description("An int value");
   conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
   conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
   conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");
   
   // parse 
   conf.parse(argc, argv);
   
   // processing the configuration here ...
   
   return 0;
}
```

## More info

### Author 

Tsz-Ho Yu (thyu413@gmail.com)


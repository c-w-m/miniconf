
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
    
## miniconf --- A quick start

### An Example

Miniconf uses a very simple API for defining a configuration for an application, a working example is shown below:

```c++
int main(int argc, char** argv)
{
   // create a Config object
   thyu::Config conf;
   
   // Setup config options 
   // Option properties, e.g. short flag
   conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
   conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
   conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");
   
   // parse 
   conf.parse(argc, argv);
   
   // processing the configuration here ...
   
   return 0;
}
```

#### Configuring an application via command line / input file

User can modify the configuration settings by command line, for example:

```bash
$ ./program --numOpt 6.28 --boolOpt true -s "another string"
```
Alternatively, a config file can also be used:

```bash
$ ./program --config settings.json -s "another string"
```

Where the content of config file "settings.json" is:

```json
{
   "numOpt": 6.28,
   "boolOpt": true, 
   "stringOpt": "this will be overwritten"
}
```
Note that command-line arguments has a higher priority so the attribute "stringOpt" in the json file will be overwritten by "-s/--stringOpt" in the command-line. 

The configurations in the above two examples should be the same when parsed by miniconf:
 
* _numOpt_ = 6.28
* _boolOpt_ = true
* _stringOpt_ = "another string"

### Accessing configuration values



## More info

### Author 

Tsz-Ho Yu (thyu413@gmail.com)


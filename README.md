
# miniconf

A minimalist C++ configuration manager

## Features

   This is a simple manager to read, store and serialize application
   settings of a C++ program. It is not meant to be fast, but it is 
   designed to make C++ programs simpler and more flexible for 
   prototyping, research, and development in general.
   
   Its main features are:

    * Define simple application settings easily
    * Parse settings from command line arguments
    * Support JSON and CSV serialization
    * Format checking, user input validation, default values, etc.
    * Generate help and usage message automatically

------------------------------------------------------------------------

## miniconf --- A quick start

#### An Example

miniconf uses a very simple API for configuring for an application, a working example is shown below:

```c++
int main(int argc, char** argv)
{
    /* create a Config object */
    miniconf::Config conf;

    /* Setup config options */
    /* Option properties, e.g. shortened flag, default values... */
    conf.option("numOpt").shortflag("n").defaultValue(3.14).required(false).description("A number value");
    conf.option("boolOpt").shortflag("b").defaultValue(false).required(true).description("A boolean value");
    conf.option("strOpt").shortflag("s").defaultValue("string").required(true).description("A string value");

    /* parse */
    bool success = conf.parse(argc, argv);
    if (success){
        /* processing the configuration here ... */
    } 
    else 
    {
        /* report error here ... */
    }
    return 0;
}
```

------------------------------------------------------------------------

#### Configuring an application via command line / input file

User can modify the configuration settings by command line, for example:

```bash
$ ./program --numOpt 6.28 --boolOpt true -s "another string"
```

Alternatively, a config file can also be used, both JSON and CSV formats are supported, the file format is determined by the input file's extension, for example:

```bash
$ ./program --config settings.json -s "overwritten"
```
or 

```bash
$ ./program --config settings.csv -s "overwritten"
```

Where the content of config file "settings.json" is:

```json
{
   "numOpt": 6.28,
   "boolOpt": true, 
   "stringOpt": "this will be overwritten"
}
```

and in settings.csv, the content will be:

```
numOpt,6.28
boolOpt,true
stringOpt,this will be overwritten
```

Note that command-line arguments has a higher priority so the attribute "stringOpt" in the json file will be overwritten by "-s/--stringOpt" in the command-line. 

The configurations in the above two examples should be the same when parsed by miniconf:
 
* _numOpt_ = 6.28
* _boolOpt_ = true
* _stringOpt_ = "overwritten"

A configuration file can also be loaded programmatically using the C++ Config::config() function, for example:

```C++
// create a Config object
miniconf::Config conf;

/* ... some parsing here if necessary ... */

// load (and possibly overwrite) the current config with the input json file
conf.config("settings.json");

```

The miniconf::Config::parse() function returns a boolean which indicates whether the parsing process is performed successfully.

------------------------------------------------------------------------

#### Nested configuration

Nested configuration is support in miniconf with JSON, for example:

```c++
   // Setup config options 
   // Option properties, e.g. short flag
   conf.option("a.b.c").shortflag("c").defaultValue("c_val").required(false).description("A nested value");
   conf.option("a.b.d").shortflag("d").defaultValue("d_val").required(false).description("A nested value");
   conf.option("a.e").shortflag("e").defaultValue("e_val").required(false).description("A nested value");
```

With the command line arguments which overrides the original default values:

```bash
./program --a.b.c overwritten_c_val
```

the parsed configuration is:

* _a.b.c_ = "overwritten_c_val"
* _a.b.d_ = "d"
* _a.e_ = "e"

and the exported JSON file it will become:

```json
{
    "a": {
        "b" : {
            "c" : "overwritten_c_val",
            "d" : "d" 
        },
        "e" : "e"
    }
}
```

------------------------------------------------------------------------

#### Accessing configuration settings

Configuration values can be read by specifying their type _implicitly_, if output data type can be determined, for example:

```c++
double n = conf["numOpt"];
bool b = conf["boolOpt"];
std::string s = conf["strOpt"];
```

or, user may also define the configuration value _explicitly_:

```c++
double n = conf["numOpt"].getNumber();
bool b = conf["boolOpt"].getBoolean();
std::string s = conf["strOpt"].getString();
```

------------------------------------------------------------------------

#### Modifying Configuration Settings

Configuration values can also be modified during runtime:

```c++
conf["numOpt"] = 9.42;
conf["strOpt"] = "fourth string";
conf["boolOpt"] = false;
```
Or one can also do explicitly

```c++
conf["numOpt"] = mimiconf::Value(12.56);
```

------------------------------------------------------------------------

#### Print current configuration summary

User may print the current configuration settings using the Config::print() function:

```c++
conf.print(); // print current settings
```

The output will be (The default output file is stdout):

```bash
[[[  CONFIGURATION  ]]]

|-------------------------|------------|--------------------------------------------------|
|           NAME          |    TYPE    |                     VALUE                        |
|-------------------------|------------|--------------------------------------------------|
| boolOpt                 | BOOLEAN    | false                                            |
| intOpt                  | INT        | 122                                              |
| numOpt                  | NUMBER     | 3.140000                                         |
| strOpt                  | STRING     | "string"                                         |
|-------------------------|------------|--------------------------------------------------|

```

------------------------------------------------------------------------

#### Automated Help / Usage Message Generation

The function Config::help() generates and displays a help message, which looks like the message below:

```
[[[  USAGE  ]]]

    example -b <BOOLEAN> [-cfg <STRING>] [-h <BOOLEAN>] [-n <NUMBER>] -s <STRING> 


[[[  HELP  ]]]

    -b, --boolOpt <REQUIRED>
        A boolean value  ( DEFAULT = false ) 

    -cfg, --config 
        Load the config file. Config format is determined by the file's extenion. If no file extension is found, default JSON loader is used  ( DEFAULT = "" ) 

    -h, --help 
        Display the help message  ( DEFAULT = false ) 

    -n, --numOpt 
        A number value  ( DEFAULT = 3.140000 ) 

    -s, --strOpt <REQUIRED>
        A string value  ( DEFAULT = "string" ) 
```


The upper "usage" section can be displayed indepdently using the Config::usage() function. 

The "auto-help" feature is enabled in miniconf by default, which means that an extra hidden option "--help/-h" is added to the configuration by default. The help message will be displayed to stdout when "--help/-h" is true. One can change the auto-help features by *Config::enableHelp()*.

------------------------------------------------------------------------

#### Serialization / programmatic config file loading

miniconf supports serialization to JSON / CSV formats:
```c++
// serialize current settings
conf.serialize("output_settings.json", Config::ExportFormat::JSON);

```
Two file formats, *Config::ExportFormat::JSON* and *Config::ExportFormat::CSV* are supported. The exported config files can be loaded back by using the "--config" argument, or the "Config::config()" function.

#### Vanilla version: JSON-less version

mimiconf requires a json parser to support JSON export and import, currently we are using picojson [GITHUB](https://github.com/kazuho/picojson) as the backend JSON parser. 

If one wants to remove the JSON dependency (to reduce code size, to remove the BSD license, etc.), one might need to uncomment the line 
```c++
#define MINICONF_JSON_SUPPORT
```
in miniconf.h

#### Extra configuration values

Unrecognized option flags are treated as "extra configuration values", they will not be neglected and are processed according to how the setting is given to the miniconfig parser:

If it is given as a command line argument, or CSV file, it will be parsed as a string value, for example:
```bash
./program --extra 123
```
The option value of flag "extra" will be a string of "123".

Alternatively if it is given as a JSON file, it will try to parse the default data type interpreted by the JSON file:
```json
{
    "extra1" : 123,
    "extra2" : "extra string"
}
```
The option value of flag "extra1" will be a number of value 123.0, and the option value of flag "extra2" will be a string of "extra string".

*Note:*

* Shortened flags are not supported and will be ignored.
* Values without any flag are not supported and will be ignored.

#### validation and logging

By default, the configuration settings and the option values given by the user will be automatically checked and validated in the  miniconf::Config::parse() function. Before parsing the argument, user can set the logging level of config checker using the miniconf::Config::log() function. 

For example, if the user wants to log all the messages:
```c++
conf.log(miniconf::Config::LogLevel::INFO);
```
On the other hand, if the user wants to log fatel errors only:
```c++
conf.log(miniconf::Config::LogLevel::ERROR);
```
The with the return value of parse() function, user might want to write a simple error checker like this:
```c++
/* set log level to "warning" which logs non-fatal warning as well */
conf.log(miniconf::Config::LogLevel::WARNING); 
/* parse the arguments, if error, print log and quit */
if (conf.parse(argc, argv)){
    // perform normal tasks...
} else {
    /* display erorr log */
    conf.log();
    exit(1);
}
```

------------------------------------------------------------------------

miniconf is licensed under the unlicense license. :)

Tsz-Ho Yu (thyu413@gmail.com)


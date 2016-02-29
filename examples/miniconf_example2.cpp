/* 
 * miniconf example 2
 *
 * author: Tsz-Ho Yu (tszhoyu@gmail.com)
 */
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
    conf.option("part1.value1").shortflag("p1v1").defaultValue("p1v1").required(false).description("Nested value example");
    conf.option("part1.value2").shortflag("p1v2").defaultValue("p1v2").required(false).description("Nested value example");
    conf.option("part1.value3").shortflag("p1v3").defaultValue(1.3).required(false).description("Nested value example");
    conf.option("part2.value1").shortflag("p2v1").defaultValue(2.1).required(false).description("Nested value example");
    conf.option("part2.subpart1.value1").shortflag("p2-1v1").defaultValue("p2-1v1").required(false).description("Nested value example");
    conf.option("part2.subpart1.value2").shortflag("p2-1v2").defaultValue("p2-1v2").required(false).description("Nested value example");
    conf.option("part2.subpart2.value1").shortflag("p2-2v1").defaultValue("p2-2v1").required(false).description("Nested value example");

    // configuration options can be adjusted:
    conf.option("strOpt").defaultValue("another string");

    // log level can be adjusted
    conf.log(miniconf::Config::LogLevel::INFO);

    // parse 
    if (conf.parse(argc, argv)){
        
        printf("Parsing is successful!\n");
       
        // access nested values
        printf("\nValue of config \"part2.subpart1.value1\" = %s\n", 
                conf["part2.subpart1.value1"].getString().c_str());

        // export file
        printf("\nSave to \"demo_settings.json\"...\n");
        conf.serialize("demo_settings.json", miniconf::Config::ExportFormat::JSON); // you may load back settings // conf.config("demo_settings.json");

        // print the configuration values
        conf.config("demo_settings.json");

    } else {
        printf("Errors in parsing!\n");
        // print log messages
        conf.log();
    }


    return 0;
}

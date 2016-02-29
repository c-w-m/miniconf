/*
 * miniconf.h
 *
 * A minimalist C++ configuration manager
 *
 * Author: Tsz-Ho Yu (tszhoyu@gmail.com)
 *
 */

/*
 * changelog:
 *
 * Version 1.0
 *     First workable version, polishing / documentation pending
 * Version 1.1
 *     Basic JSON export/import
 * Version 1.2
 *     Basic CSV export/import
 * Version 1.3 
 *     Code clean up
 * Version 1.4
 *     Support nested JSON
 * Version 1.5
 *     JSON-less version
 *
 */

#ifndef __MINICONF_H__
#define __MINICONF_H__

/* Comment the line below to disable JSON support */
#define MINICONF_JSON_SUPPORT

#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

#ifdef MINICONF_JSON_SUPPORT
#include "picojson.h"
#endif

namespace miniconf
{

    /* A flexible container for multiple data type
     *
     * miniconf::Value is a flexible container for int, double, bool and char array. The 
     * actual value is stored in a buffer declared during assignment. An extra "unknown"
     * type is also defined for empty, or invalid value. 
     */
    class Value
    {
        public:

            // DataType specifies the datatype the instance is storing
            enum class DataType {
                UNKNOWN,
                INT,
                NUMBER,
                BOOL,
                STRING
            };

            /* Default constructors and assignments for Value, "unknown" type is assigned
             * to the instnace. It is suggested to use other constructor to assign valid
             * value when the instance is instantiated.
             */
            Value();

            // Copy assignment constructor
            Value(const Value& other);
            
            // Move assignment constructor 
            Value(Value&& other);

            // Assignment operator
            Value& operator=(const Value& other);

            // Move assignment operator
            Value& operator=(Value&& other);

            // Default destructor, releases buffer used to hold the value
            ~Value();

            // Constructs a Value instance from an integer 
            explicit Value(const int& other);
            
            // Constructs a Value instance from a floating point
            explicit Value(const double& other);
            
            // Constructs a Value instance from a boolean
            explicit Value(const bool& other);

            // Constructs a Value instance from a char array
            explicit Value(const char* other);
            
            // Constructs a Value instance from a std::string
            explicit Value(const std::string& other);
           
            // Assigns an integer to a Value instance
            Value& operator=(const int& other);
            
            // Assigns a floating point to a Value instance
            Value& operator=(const double& other);
            
            // Assigns a boolean to a Value instance
            Value& operator=(const bool& other);
            
            // Assigns a char array to a Value instance
            Value& operator=(const char* other);
            
            // Assigns a std::string to a Value instance
            Value& operator=(const std::string& other);
           
            // Casts a Value to an integer
            explicit operator int() const;

            // Casts a Value to a floating point number
            explicit operator double() const;

            // Casts a Value to a boolean
            explicit operator bool() const;

            // Casts a Value to a char array
            explicit operator char*() const;

            // Casts a Value to a std::string
            explicit operator std::string() const;
           
            // Explicitly gets an integer from a Value instance
            int getInt() const;
            
            // Explicitly gets a floating point number from a Value instance
            double getNumber() const;
            
            // Explicitly gets a boolean from a Value instance
            bool getBoolean() const;

            // Explicitly gets a char array from a Value instance
            char* getCharArray() const;

            // Explicitly gets a std::string from a Value instance
            std::string getString() const;

            // Serializes the value to a string
            std::string print();

            // Serializes the data type of the current value to a string, mainly for debugging purpose
            std::string printType();

            // Gets the data type of the current value
            DataType type();

            // Checks if the value is empty (unknown)
            bool isEmpty();

            // Generates an unknown (empty) Value object
            static Value unknown();

        private:

            // Moves value data from a pointer
            Value& moveData(char*& src, const size_t size, const DataType type);

            // Copies value data from a pointer
            Value& copyData(const char* src, const size_t size, const DataType& type);

            // Clears allocated value data
            void clearData();

            // It stores the data type of the current value
            DataType _type;

            // Number of bytes allocated to the buffer
            size_t _size;

            // The pointer to the value buffer
            char* _data;
    };

    /*
     * A Config object describes the configuration settings of an 
     * application. It contains a list of options which can be parsed from 
     * program arguments or from a configuration file (e.g. JSON).
     *
     * Configuration options are defined via the Config class's API. The
     * data type of each option is determined during compile time. User
     * can modify the values of the configuration options during run-time.
     * An option is invoked by a option flag, e.g. "--option" or "-o" in 
     * the command line arguments, or their corresponding attribute values
     * in a configuration file. Configuration file support can also be 
     * disabled manually.
     *
     * Basic sanity checks, such as default values and format checks are 
     * also performed automatically. User can display warnings and error
     * messages generated from the parser. 
     *
     * Help and usage messages can also be generated automatically from 
     * the application config settings (This feature can also be
     * disabled manually). 
     *
     */
    class Config
    {
        public:

            /* Level of error / log messages
             * 
             * The LogLevel enums defines the level of log / error messages generated
             * by the config / argument parser. 
             * * INFO - Normal log message
             * * WARNING = There are something thta need to be fixed, but not fatal
             * * ERROR  = Fatal error which  will cause undefined behavior
             * * NONE = No message will be generated, all logging is suppressed.
             */
            enum class LogLevel {
                INFO,
                WARNING,
                ERROR,
                NONE
            };

            /* File format for serialization 
             *
             * User can either serialize the current configuration, or 
             * write one config file manually using external editors.
             */
#ifdef MINICONF_JSON_SUPPORT
            enum class ExportFormat {
                JSON,
                CSV
            };
#else
            enum class ExportFormat {
                CSV
            };
#endif

            /* Option member class which describe the properties of a configuration option.
             * 
             * A complete configuration setting is composed of multiple options, 
             * each option is responsible for one property. Each option can be 
             * invoked by its flag (e.g. --flag) or shortened flag (e.g. -f). 
             * Some options are required to be set manually, while some options
             * have default values such that user input is not always required.
             * 
             * The data type of an option is determined from its defaultValue.
             */
            class Option;

            // Default constructor, no option is defined except the default "help" and "config"
            Config();

            // Default destructor
            ~Config();

            /* The main parsing function for input command line
             * @argc The number of input arguments
             * @argv An array of char array which contains the arguments, the first argument
             * should be the name of the executable
             * @return True when parsing is successful, False when parsing error(s) occur
             */
            bool parse(int argc, char** argv);
            
            // Creates a new configuration option, which is uniquely identified by its flag
            Config::Option& option(const std::string& flag);

            // Removes an option
            bool remove(const std::string& flag);

            // Checks if the option value is defined in the current configuration
            bool contains(const std::string& flag);
            
            // Sets a short description of the current application.
            void description(const std::string& desc);

            // Prints the current configuration settings
            void print(FILE* fd = stdout);

            // Prints the current log messages
            void log(FILE* fd = stdout);

            // Sets the logging level, NO
            void log(const LogLevel logType);

            // display the log message
            void verbose(bool value);
            
            /*Checks config format design and reports errors if necessary
             *
             * This function will validate the configuration format set by the user agsinst
             * invalid settings (e.g. optional arguments without a default value). This will
             * be called during the parse() process.
             *  
             * @return The most severe level of issue detected by the process
             */
            LogLevel checkFormat();

            /* Validates user input after parsing / loading config file
             * 
             * This validates the configuration after user input, e.g. all option values are 
             * defined and not of the UNKNOWN type. This will be called automatically 
             * after the option values have been parsed in the parse() function. 
             *
             * @return The most severe level of issue detected by the process
             */
            LogLevel validate();

            /* Accesses the configuration value
             *
             * If the configuration value does not exist, an empty Value object is returned. 
             */
            Value& operator[](const std::string& flag);

            /* Load the configuration settings via a config file
             * 
             * This function loads a config file, if the config file has been specified in 
             * command line arguments, this will be called automatically in "parse()" function.
             *
             * @configPath the input configuration file path
             */
            void config(const std::string& configPath);

            /* Serializes the current configuration
             *
             * Currently JSON and CSV are supported.
             */
#ifdef MINICONF_JSON_SUPPORT
            std::string serialize(const std::string& serializeFilePath = "", ExportFormat format = ExportFormat::JSON, bool pretty = true);
#else
            std::string serialize(const std::string& serializeFilePath = "", ExportFormat format = ExportFormat::CSV, bool pretty = true);
#endif

            // Enables automatically generated help message (--help/-h)
            void enableHelp(bool enabled = true);

            // Ennables setting via external config file (--config/-cfg)
            void enableConfig(bool enabled = true);

            // Prints usage of this program's configuration options
            void usage(FILE* fd = stdout);

            // Prints a automatically generated help message 
            void help(FILE* fd = stdout);

        private:

            /* Types of command line arguments
             *
             * "UNKNOWN" indicates a parsing error
             * A "FLAG" is command line flag which starts with "--"
             * A "SHORTFLAG" is shortened flag which starts with "-"
             * "VALUE" represent an argument value to be parsed
             */
            enum class TokenType {
                UNKNOWN,    // parsing error
                FLAG,       // a long flag, e.g. "--setting"
                SHORTFLAG,  // a short flag, e.g. "-s"
                VALUE       // value, e.g. 123, "hello", true
            };

            // Sets default values to option values, used in parse() for initialization
            void setDefaultValues();

            // get current token type
            TokenType getTokenType(const char* token);

            // since long flag is the key for the option directory,
            // this function transltes short flag to long flag
            std::string translateShortflag(const std::string& shortflag);

            // search for options using token
            Option* getOption(const char* token, Config::TokenType tokenType);

            // determine is a flag is defined in the config
           bool findOption(const std::string& flag);

            // parse a token into Value
            Value parseValue(const char* token, Value::DataType dataType);

#ifdef MINICONF_JSON_SUPPORT
            // load json config string
            bool loadJSON(const std::string& JSONStr);

            // parse a json value
            bool parseJSON(const picojson::value *v, const std::string& flag); 

            // load a value from json
            bool getJSONValue(const picojson::value *v, const std::string& flag); 
#endif

            // load csv config string
            bool loadCSV(const std::string& CSVStr);

            // internal function for adding log messages
            void log(LogLevel logType, const std::string& token, const std::string& msg);

            // this map stores configuration format design, e.g. flag, default values.
            std::map<std::string, Option> _options;

            // this map stores the values parsed form user input
            std::map<std::string, Value> _optionValues;

            // this is a stack of log messages
            std::vector<std::string> _log;

            // switch for verbose
            bool _verbose;  
            
            // log level setting
            LogLevel _logLevel; 
            
            // program name stripped from command line
            std::string _exeName; 
            
            // the program's description
            std::string _description; 
            
            // switch for auto help mesage
            bool _autoHelp; 
            
            // switch for enable loading configuration
            bool _loadConfig; 

    };

    /*
     * Describe one configuration options
     * A complete configuration setting is composed of multiple options, 
     * each option is responsible for one property. Each option can be 
     * invoked by its flag (e.g. --flag) or shortened flag (e.g. -f). 
     * Some options are required to be set manually, while some options
     * have default values such that user input is not always required.
     * 
     * The data type of an option is determined from its defaultValue.
     */
    class Config::Option
    {
        public:

            /* Default constructor that creates a empty option
             *
             * Use should use Config::option(flag) to create an new option
             */
            Option();

            // Default destructor
            ~Option();

            // Sets the flag of an option
            Config::Option& flag(const std::string& flag);

            // Sets the short flag of an option
            Config::Option& shortflag(const std::string& shortflag);

            // Sets the description of an option
            Config::Option& description(const std::string& description);

            // Sets the default value of an option from a Value object
            Config::Option& defaultValue(const Value& defaultValue);

            // Sets the default value of an option from an integer
            Config::Option& defaultValue(const int& defaultValue);
            
            // Sets the default value of an option from a floating point
            Config::Option& defaultValue(const double& defaultValue);
            
            // Sets the default value of an option from a boolean
            Config::Option& defaultValue(const bool& defaultValue);
            
            // Sets the default value of an option from a char array
            Config::Option& defaultValue(const char* defaultValue);
            
            // Sets the default value of an option from a string
            Config::Option& defaultValue(const std::string& defaultValue);

            // Makes an option to be required or optional
            Config::Option& required(const bool required);

            /* Sets an option to be hidden, so it will be erased after parsing
             *
             * This will be used for the default "help" and "config" options. 
             */
            Config::Option& hidden(const bool hidden);

            // Prints the flag of an option as a string
            std::string flag();

            // Prints the shortflag of an option as a string
            std::string shortflag();

            // Prints the description of an option
            std::string description();

            // Returns the default value of an option
            Value defaultValue();

            // Checks if the option is required or optional
            bool required();

            // Gets the data type 
            Value::DataType type();

            // Checks if an option is hidden
            bool hidden();

        private:

            // Flag of the option
            std::string     _flag;          
            
            // Shortened flag of the option
            std::string     _shortflag;     
            
            // Description of the option
            std::string     _description;   
            
            // Default value of the option
            Value           _defaultValue;
            
            // Option is required to be specified
            bool            _required;
            
            // Option is hidden
            bool            _hidden;

    };

    /*********************************************************************/
    /*********************************************************************/
    /*********************** IMPLEMENTATION BELOW ************************/
    /*********************************************************************/
    /*********************************************************************/

    // Value
    Value::Value() : _type(DataType::UNKNOWN), _size(0), _data(nullptr)
    {}

    Value::Value(const Value& other) : Value()
    {
        copyData(other._data, other._size, other._type);
    }

    Value::Value(Value&& other) : Value()
    {
        moveData(other._data, other._size, other._type);
    }


    Value& Value::operator=(const Value& other)
    {
        clearData();
        return copyData(other._data, other._size, other._type);
    }

    Value& Value::operator=(Value&& other)
    {
        return moveData(other._data, other._size, other._type);
    }

    Value::~Value()
    {
        clearData();
    }

    //  int
    Value::Value(const int& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
    }

    Value& Value::operator=(const int& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
    }

    Value::operator int() const
    {
        return *reinterpret_cast<int*>(_data);
    }

    int Value::getInt() const
    {
        return *reinterpret_cast<int*>(_data);
    }

    //  number (floating point)
    Value::Value(const double& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
    }

    Value& Value::operator=(const double& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
    }

    Value::operator double() const
    {
        return *reinterpret_cast<double*>(_data);
    }

    double Value::getNumber() const
    {
        return *reinterpret_cast<double*>(_data);
    }


    //  bool
    Value::Value(const bool& other) : Value()
    {
        copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::BOOL);
    }

    Value& Value::operator=(const bool& other)
    {
        clearData();
        return copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::BOOL);
    }

    Value::operator bool() const
    {
        return *reinterpret_cast<bool*>(_data);
    }

    bool Value::getBoolean() const
    {
        return *reinterpret_cast<bool*>(_data);
    }

    //  char array
    Value::Value(const char* other) : Value()
    {
        copyData(other, strlen(other) + 1, DataType::STRING);
    }

    Value& Value::operator=(const char* other)
    {
        clearData();
        return copyData(other, strlen(other) + 1, DataType::STRING);
    }

    Value::operator char*() const
    {
        return reinterpret_cast<char*>(_data);
    }

    char* Value::getCharArray() const
    {
        return reinterpret_cast<char*>(_data);
    }

    //  std::string
    Value::Value(const std::string& other) : Value()
    {
        copyData(other.c_str(), other.size() + 1, DataType::STRING);
    }

    Value& Value::operator=(const std::string& other)
    {
        clearData();
        return copyData(other.c_str(), other.size() + 1, DataType::STRING);
    }

    Value::operator std::string() const
    {
        return std::string(reinterpret_cast<char*>(_data));
    }

    std::string Value::getString() const
    {
        return std::string(reinterpret_cast<char*>(_data));
    }

    // print function
    std::string Value::print()
    {
        const int slen = 31;
        char tempStr[slen + 1];
        std::string outStr;
        switch (_type) {
            case DataType::UNKNOWN:
                snprintf(tempStr, slen, "null");
                break;
            case DataType::INT:
                snprintf(tempStr, slen, "%d", getInt());
                outStr = std::string(tempStr);
                break;
            case DataType::NUMBER:
                snprintf(tempStr, slen, "%f", getNumber());
                outStr = std::string(tempStr);
                break;
            case DataType::BOOL:
                snprintf(tempStr, slen, "%s", getBoolean() ? "true" : "false");
                outStr = std::string(tempStr);
                break;
            case DataType::STRING:
                outStr = "\"" + std::string(getCharArray()) + "\"";
                break;
            default:
                break;
        }
        return outStr;
    }

    // return data type
    Value::DataType Value::type()
    {
        return _type;
    }

    // check empty
    bool Value::isEmpty()
    {
        return (_data == nullptr || _type == DataType::UNKNOWN);
    }

    // generate unknown value
    Value Value::unknown()
    {
        return Value();
    }

    // print value data type
    std::string Value::printType()
    {
        std::string outStr;
        const int slen = 15;
        char tempStr[slen + 1];
        switch (_type) {
            case DataType::UNKNOWN:
                snprintf(tempStr, slen, "UNKNOWN");
                break;
            case DataType::INT:
                snprintf(tempStr, slen, "INT");
                break;
            case DataType::NUMBER:
                snprintf(tempStr, slen, "NUMBER");
                break;
            case DataType::BOOL:
                snprintf(tempStr, slen, "BOOLEAN");
                break;
            case DataType::STRING:
                snprintf(tempStr, slen, "STRING");
                break;
            default:
                break;
        }
        return std::string(tempStr);
    }

    // move and copy function
    Value& Value::moveData(char*& src, const size_t size, const DataType type)
    {
        _type = type;
        _size = size;
        _data = src;
        src = nullptr;
        return *this;
    }

    // internal use
    Value& Value::copyData(const char* src, const size_t size, const DataType& type)
    {
        char* newData = new char[size];
        memcpy(newData, src, size);
        return moveData(newData, size, type);
    }

    // internal use
    void Value::clearData()
    {
        if (_size != 0 && _data != nullptr) {
            delete _data;
            _data = nullptr;
        }
    }

    // Option
    Config::Option::Option() : _flag(), _shortflag(), _description(), _defaultValue(Value::unknown()), _required(false), _hidden(false)
    {}

    Config::Option::~Option()
    {}

    Config::Option& Config::Option::flag(const std::string& flag)
    {
        _flag = flag;
        return *this;
    }

    Config::Option& Config::Option::shortflag(const std::string& shortflag)
    {
        _shortflag = shortflag;
        return *this;
    }

    Config::Option& Config::Option::description(const std::string& description)
    {
        _description = description;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const Value& defaultValue)
    {
        _defaultValue = defaultValue;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const int& defaultValue)
    {
        _defaultValue = static_cast<int>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const double& defaultValue)
    {
        _defaultValue = static_cast<double>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const bool& defaultValue)
    {
        _defaultValue = static_cast<bool>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const char* defaultValue)
    {
        _defaultValue = defaultValue;
        return *this;
    }

    Config::Option& Config::Option::defaultValue(const std::string& defaultValue)
    {
        _defaultValue = static_cast<std::string>(defaultValue);
        return *this;
    }

    Config::Option& Config::Option::required(const bool required)
    {
        _required = required;
        return *this;
    }

    Config::Option& Config::Option::hidden(const bool hidden)
    {
        _hidden = hidden;
        return *this;
    }

    std::string Config::Option::flag()
    {
        return _flag;
    }

    std::string Config::Option::shortflag()
    {
        return _shortflag;
    }

    std::string Config::Option::description()
    {
        return _description;
    }

    Value Config::Option::defaultValue()
    {
        return _defaultValue;
    }

    bool Config::Option::required()
    {
        return _required;
    }

    bool Config::Option::hidden()
    {
        return _hidden;
    }

    Value::DataType Config::Option::type()
    {
        return _defaultValue.type();
    }

    Config::Config() :
        _verbose(false),
        _logLevel(Config::LogLevel::WARNING),
        _exeName(""),
        _description(""),
        _autoHelp(true),
        _loadConfig(true)
    {
        enableHelp(true); // set auto help to true
        enableConfig(true); // set auto config to true
    }

    Config::~Config()
    {
        _options.clear();
        _optionValues.clear();
        _log.clear();
    }

    Config::Option& Config::option(const std::string& flag)
    {
        _options.insert(std::make_pair(flag, Config::Option().flag(flag)));
        return _options[flag];
    }

    bool Config::remove(const std::string& flag)
    {
        if (findOption(flag)){
            _options.erase(flag); 
            return true; 
        }
        return false; 
    }

    void Config::setDefaultValues()
    {
        for (auto && o : _options) {
            _optionValues[o.first] = o.second.defaultValue();
        }
    }

    void Config::log(const LogLevel logType)
    {
        _logLevel = logType;
    }

    void Config::log(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "PARSE LOG");
        for (auto && logline : _log) {
            fprintf(fd, "%s\n", logline.c_str());
        }
    }

    void Config::log(Config::LogLevel logType, const std::string& token, const std::string& msg)
    {
        // do don't anything if log level is low
        if (logType < _logLevel) {
            return;
        }
        const int tagWidth = 16;
        char tag[tagWidth + 1];
        char format[tagWidth + 1];
        std::string logString = "";
        snprintf(format, tagWidth, "<<<%%%ds>>>", tagWidth - 7);
        switch (logType) {
            case Config::LogLevel::INFO:
                snprintf(tag, tagWidth, format, "INFO");
                break;
            case Config::LogLevel::WARNING:
                snprintf(tag, tagWidth, format, "WARNING");
                break;
            case Config::LogLevel::ERROR:
                snprintf(tag, tagWidth, format, "ERROR");
                break;
            default:
                break;
        }
        logString = std::string(tag) + " Input \"" + token + "\" : " + msg;
        _log.emplace_back(logString);
        if (_verbose) {
            fprintf(stdout, "%s\n", logString.c_str());
        }
    }

    Config::TokenType Config::getTokenType(const char* token)
    {
        // get token type:
        // starts with "--" - flag
        // starts with "-" - shortflag/value
        // otherwise value
        size_t tlen = strlen(token);
        if (tlen == 0) return TokenType::UNKNOWN;
        if (token[0] == '-') {
            // check if it can be converted to a number
            char* endptr = nullptr;
            std::strtod(token, &endptr);
            if (*endptr == '\0' && endptr != token) {
                return TokenType::VALUE;
            }
            return (token[1] == '-') ? TokenType::FLAG : TokenType::SHORTFLAG;
        }
        return TokenType::VALUE;
    }

    std::string Config::translateShortflag(const std::string& shortflag)
    {
        auto found = _options.begin();
        auto end = _options.end();
        for (; found != end; ++found) {
            if (found->second.shortflag() == shortflag) {
                break;
            }
        }
        if (found != end) {
            return found->first;
        }
        return shortflag;
    }

    bool Config::findOption(const std::string& flag)
    {
        return (_options.find(flag) != _options.end());
    }

    Config::Option* Config::getOption(const char* token, Config::TokenType tokenType)
    {
        std::string flag = "";
        if (tokenType == TokenType::FLAG) {
            flag = std::string(token + 2);
        } else if (tokenType == TokenType::SHORTFLAG) {
            flag = translateShortflag(std::string(token + 1));
        }
        if (flag.empty()) {
            return nullptr;
        }
        if (findOption(flag)) {
            return &(_options[flag]);
        }
        return nullptr;
    }

    Value Config::parseValue(const char* token, Value::DataType dataType)
    {
        if (dataType == Value::DataType::INT) {
            int v;
            int success = sscanf(token, "%d", &v);
            return (success == 1) ? Value(v) : Value::unknown();
        }
        if (dataType == Value::DataType::NUMBER) {
            double v;
            int success = sscanf(token, "%lf", &v);
            return (success == 1) ? Value(v) : Value();

        }
        if (dataType == Value::DataType::BOOL) {
            std::string v(token);
            if (v == "false" || v == "False" || v == "FALSE" || v == "F" || v == "f") {
                return Value(false);
            }
            return Value(true);

        }
        if (dataType == Value::DataType::STRING) {
            return Value(token);
        }
        return Value::unknown(); // fool-proof, return an unknown
    }

    static Config::LogLevel worseLevel(const Config::LogLevel& a, const Config::LogLevel& b)
    {
        return ((a) < (b)) ? (b) : (a);
    }

    Config::LogLevel Config::checkFormat()
    {
        LogLevel errorLv = LogLevel::INFO;
        for (auto && opt : _options) {
            Option& o = opt.second;
            // check for error
            if (!o.required() && o.defaultValue().isEmpty()) {
                log(LogLevel::ERROR, o.flag(), "default value is not defined");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
            for (auto& opt2 : _options) {
                Option& o2 = opt2.second;
                if ((o.flag() != o2.flag()) && (o.shortflag() == o2.shortflag()) && !(o.shortflag().empty())) {
                    log(LogLevel::ERROR, o.flag(), "duplicate short flags (" + o2.shortflag() + ")");
                    errorLv = worseLevel(errorLv, LogLevel::ERROR);
                }
            }
            // check for warnings
            if (o.description().empty()) {
                log(LogLevel::WARNING, o.flag(), "no description text for argument");
                errorLv = worseLevel(errorLv, LogLevel::WARNING);
            }
            if (o.shortflag().empty()) {
                log(LogLevel::WARNING, o.flag(), "no short flag is provided");
                errorLv = worseLevel(errorLv, LogLevel::WARNING);
            }
        }
        if (_description.empty()) {
            log(LogLevel::WARNING, "", "No program description text is provided");
            errorLv = worseLevel(errorLv, LogLevel::WARNING);
        }
        return errorLv;
    }

    // Validate User Input
    Config::LogLevel Config::validate()
    {
        LogLevel errorLv = LogLevel::INFO;

        // remove all the hidden values
        for (auto&& opt: _options){
            if (opt.second.hidden() && contains(opt.first)){
                _optionValues.erase(opt.first);
            }
        }

        // scan for all option vlaues 
        for (auto && val : _optionValues) {
            if (val.second.isEmpty()) {
                log(LogLevel::ERROR, val.first, "option contains invalid value");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
        }

        // scan for all remaining options are defined
        for (auto && opt : _options) {
            if (!contains(opt.first) && !opt.second.hidden()) {
                log(LogLevel::ERROR, opt.first, "option is undefined");
                errorLv = worseLevel(errorLv, LogLevel::ERROR);
            }
        }
        return errorLv;
    }

    bool Config::parse(int argc, char **argv)
    {
        // Extract executable name
        _exeName = std::string(argv[0]);
        size_t lastslash = _exeName.find_last_of("\\/");
        if (lastslash != std::string::npos) {
            _exeName = _exeName.substr(lastslash + 1);
        }

        // check format of the option parser
        // if fatal error occurs and log level is not "NONE" (NONE = ignore errors)
        LogLevel checkFormatResult = checkFormat();
        if (checkFormatResult >= LogLevel::ERROR && _logLevel <= LogLevel::ERROR) {
            log();
            printf("\nFatal Error: Option format validation failed, abort.\n\n");
            return false;
        }

        // define a wildcard option to capture "stray" option values (values without a flag)
        // string argument by default
        Config::Option wildcard;
        wildcard.defaultValue("");

        // Value Precedence:
        // (1) Default Value
        // (2) Config File Settings (overwrites default values)
        // (3) Command Line Arguments (overwrites default values and config file)

        // * Set Default Values
        setDefaultValues();

        // * Load Config File before scanning for other arguments
        // case 1: only config file is defined, flag is not necessary
        // case 2: check if config flag has been defined
        if (_loadConfig) {
            for (int i = 1; i < argc - 1; ++i) {
                std::string flag = std::string(argv[i]);
                std::string value = std::string(argv[i + 1]);
                TokenType ttype = getTokenType(argv[i + 1]);
                if (( flag == "--config" || flag == "-cfg" ) && (ttype == TokenType::VALUE)) {
                    config(value);
                }
            }
        }

        // start normal parsing
        Option* currentOption = nullptr;
        for (int i = 1; i < argc; ++i) {
            TokenType currentTokenType = getTokenType(argv[i]);
            if (currentTokenType == TokenType::UNKNOWN) {
                log(LogLevel::ERROR, std::string(argv[i]), "unknown input");
            } else if (currentTokenType == TokenType::FLAG || currentTokenType == TokenType::SHORTFLAG) {
                currentOption = getOption(argv[i], currentTokenType);
                if (!currentOption) {
                    log(LogLevel::WARNING, std::string(argv[i]), "unrecognized flag");
                    if (currentTokenType == TokenType::FLAG) {
                        wildcard.flag(std::string(argv[i] + 2));
                        currentOption = &wildcard;
                    }
                }
                // special case - if the option type is bool, set to true by default
                if (currentOption && currentOption->type() == Value::DataType::BOOL) {
                    _optionValues[currentOption->flag()] = true;
                }
            } else if (currentTokenType == TokenType::VALUE) {
                if (currentOption) {
                    // parse the value according to default data type
                    Value newValue = parseValue(argv[i], currentOption->type());
                    // if value cannot be parsed
                    if (newValue.isEmpty()) {
                        log(LogLevel::WARNING, std::string(argv[i]), "unvalid value type is provided");
                    } else {
                        // assign parsed values
                        _optionValues[currentOption->flag()] = parseValue(argv[i], currentOption->type());
                        log(LogLevel::INFO, std::string(argv[i]), "value parsed successfully");
                    }
                    // reset current option flag -> ready for a new flag
                    currentOption = nullptr;
                } else {
                    // stray arguments, ignore
                    log(LogLevel::WARNING, std::string(argv[i]), "unassociated argument is not stored");
                }
            }
        }

        // if contains help and auto-help is enabled, display help message
        if (contains("help") && _optionValues["help"].getBoolean() && _autoHelp) {
            help();
        }

        // validate user inputs
        // remove all hidden options
        // reserved words should not be displayed as normal config values

        // if fatal error occurs and log level is not "NONE" (NONE = ignore errors)
        LogLevel validateResult = validate();
        if (validateResult >= LogLevel::ERROR && _logLevel <= LogLevel::ERROR) {
            log();
            printf("\nFatal Error: Option format validation failed, abort.\n\n");
            return false;
        }

        return true;
    }

    void Config::help(FILE* fd)
    {
        // print program description
        if (!_description.empty()) {
            fprintf(fd, "\n");
            if (!_exeName.empty()) {
                fprintf(fd, "[[[  %s  ]]]\n\n    ", _exeName.c_str());
            }
            fprintf(fd, "%s\n\n", _description.c_str());
        }

        // print usage
        usage();
        // print help
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "HELP");
        for (auto && opt : _options) {
            Option& o = opt.second;
            // print short
            fprintf(fd, "    ");
            if (!o.shortflag().empty()) {
                fprintf(fd, "-%s, ", o.shortflag().c_str());
            }
            // print long
            fprintf(fd, "--%s ", o.flag().c_str());
            // required
            if (o.required()) {
                fprintf(fd, "<REQUIRED>");
            }
            fprintf(fd, "\n");
            // print description
            fprintf(fd, "        ");
            if (!o.description().empty()) {
                fprintf(fd, "%s ", o.description().c_str());
            }
            // default value
            if (!o.defaultValue().isEmpty() && !o.hidden()) {
                fprintf(fd, " ( ");
                fprintf(fd, "DEFAULT = %s", o.defaultValue().print().c_str());
                fprintf(fd, " ) ");
            }
            fprintf(fd, "\n\n");
        }
    }

    void Config::usage(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "USAGE");
        char exeTag[256];
        snprintf(exeTag, 256 - 1, "    %s ", (_exeName.empty()) ? ("<executable>") : (_exeName.c_str()));
        fprintf(fd, "%s", exeTag);
        int lineWidth = 0;
        for (auto && opt : _options) {
            Option& o = opt.second;
            char argTag[512];
            snprintf(argTag, 512 - 1, "%s%s%s <%s>%s",
                    o.required() ? "" : "[",
                    o.shortflag().empty() ? "--" : "-",
                    o.shortflag().empty() ? o.flag().c_str() : o.shortflag().c_str(),
                    o.defaultValue().printType().c_str(),
                    o.required() ? "" : "]");
            if (lineWidth + strlen(argTag) >= 80 - 1 - strlen(exeTag)) {
                fprintf(fd, "\n%*s", static_cast<int>(strlen(exeTag)), " ");
                lineWidth = 0;
            }
            fprintf(fd, "%s ", argTag);
            lineWidth += strlen(argTag);
        }
        fprintf(fd, "\n\n");
    }

    void Config::description(const std::string& desc)
    {
        _description = desc;
    }

    void Config::enableConfig(bool enabled)
    {
        _loadConfig = enabled;
        if (_autoHelp && !findOption("config")) {
            option("config").
                shortflag("cfg").
                defaultValue("").
                description("Input configuration file (JSON/CSV)").
                required(false).hidden(true);
        } else {
            if (findOption("config")) {
                _options.erase("config");
            }
        }

    }

    void Config::enableHelp(bool enabled)
    {
        _autoHelp = enabled;
        if (_autoHelp && !findOption("help")) {
            option("help").
                shortflag("h").
                defaultValue(false).
                description("Display the help message").
                required(false).hidden(true);
        } else {
            if (findOption("help")) {
                _options.erase("help");
            }
        }
    }

    void Config::verbose(bool value)
    {
        _verbose = value;
    }

    bool Config::contains(const std::string& flag)
    {
        return _optionValues.find(flag) != _optionValues.end();
    }

    Value& Config::operator[](const std::string& flag)
    {
        return _optionValues[flag];
    }

    void Config::print(FILE* fd)
    {
        fprintf(fd, "\n[[[  %s  ]]]\n\n", "CONFIGURATION");

        printf("|-------------------------|------------|--------------------------------------------------|\n");
        printf("|           NAME          |    TYPE    |                     VALUE                        |\n");
        printf("|-------------------------|------------|--------------------------------------------------|\n");
        for (auto && v : _optionValues) {
            if (_options.find(v.first.c_str()) != _options.end()) {
                fprintf(fd, "| %-23s | %-10s | %-48s |\n", v.first.c_str(), v.second.printType().c_str(), v.second.print().c_str());
            } else {
                fprintf(fd, "| %-23s | %-10s | %-48s |\n", v.first.c_str(), (v.second.printType() + "*").c_str(), v.second.print().c_str());
            }
        }
        printf("|-------------------------|------------|--------------------------------------------------|\n");
        printf("\n");
    }

    std::string Config::serialize(const std::string& serializeFilePath, ExportFormat format, bool pretty)
    {
        std::stringstream ss;
        std::string outStr;

        // extract extension
        std::string extension = "";
        size_t lastDot = serializeFilePath.find_last_of(".");
        if (lastDot != std::string::npos) {
            extension = serializeFilePath.substr(lastDot + 1);
        }
#ifdef MINICONF_JSON_SUPPORT
        if (extension == "json" || extension == "JSON"){
            format = ExportFormat::JSON; 
        } else if (extension == "csv" || extension == "CSV"){
            format = ExportFormat::CSV; 
        } else {
            format = ExportFormat::CSV; 
        }
#else
        format = ExportFormat::CSV; 
#endif

        // serialize JSON
#ifdef MINICONF_JSON_SUPPORT
        if (format == ExportFormat::JSON) {
            picojson::value outObj = picojson::value(picojson::object());
            for (auto v: _optionValues){
                std::vector<std::string> flagTokens;
                std::stringstream ss(v.first);
                // tokenize
                while (ss.good()){
                    std::string tempToken;
                    std::getline(ss, tempToken, '.');
                    flagTokens.emplace_back(tempToken);
                }
                // parse
                if (flagTokens.size() > 1){
                    picojson::value* thisObj = &outObj;
                    for (size_t i = 0; i < flagTokens.size(); ++i){
                        if (i != flagTokens.size() - 1){
                            if (thisObj->get<picojson::object>().find(flagTokens[i]) == thisObj->get<picojson::object>().end()){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(picojson::object());
                            }
                            thisObj = &(thisObj->get<picojson::object>()[flagTokens[i]]);
                        } else {
                            if (v.second.type() == Value::DataType::INT){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(static_cast<double>(v.second.getInt()));
                            } else if (v.second.type() == Value::DataType::NUMBER){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getNumber());
                            } else if (v.second.type() == Value::DataType::BOOL){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getBoolean());
                            } else if (v.second.type() == Value::DataType::STRING){
                                thisObj->get<picojson::object>()[flagTokens[i]] = picojson::value(v.second.getString());
                            } 
                        
                        }
                    }
                }else{
                    if (outObj.get<picojson::object>().find(flagTokens[0]) == outObj.get<picojson::object>().end()){
                        if (v.second.type() == Value::DataType::INT){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(static_cast<double>(v.second.getInt()));
                        } else if (v.second.type() == Value::DataType::NUMBER){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getNumber());
                        } else if (v.second.type() == Value::DataType::BOOL){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getBoolean());
                        }  else if (v.second.type() == Value::DataType::STRING){
                            outObj.get<picojson::object>()[flagTokens[0]] = picojson::value(v.second.getString());
                        } 
                    }
                }
            }
            outStr = outObj.serialize(true);
        }
#endif

        // serialize CSV
        if (format == ExportFormat::CSV) {
            for (auto opt = std::begin(_optionValues); opt != std::end(_optionValues); ++opt) {
                std::string flag = opt->first;
                std::string val = opt->second.print();
                if (opt->second.type() == Value::DataType::STRING){
                    // remove "" from string
                    if (val.size() >= 2){ 
                        val = val.substr(1, val.size()-2);
                    }
                }
                ss << flag << "," << val << std::endl; 
            }
            outStr = ss.str();
        }

        // write out file
        if (!serializeFilePath.empty()) {
            std::ofstream ofd(serializeFilePath);
            if (ofd.good()) {
                ofd << outStr;
                ofd.close();
            }
        }
        return outStr;
    }

    void Config::config(const std::string& configPath)
    {
        // read content of the file
        std::ifstream ifd(configPath, std::ios::in | std::ios::binary);
        std::string configContent = "";
        if (ifd) {
            configContent = std::string((std::istreambuf_iterator<char>(ifd)), std::istreambuf_iterator<char>());
        }

        // extract extension
        std::string extension = "";
        size_t lastDot = configPath.find_last_of(".");
        if (lastDot != std::string::npos) {
            extension = configPath.substr(lastDot + 1);
        }

        // load config according to extension
        // default is json
#ifdef MINICONF_JSON_SUPPORT
        if (extension == "json" || extension == "JSON") {
            loadJSON(configContent);
            return;
        } else if (extension == "csv" || extension == "CSV") {
            loadCSV(configContent);
            return;
        } else {
            loadJSON(configContent);
        }
#else
        loadCSV(configContent);
#endif

        return;
    }

    bool Config::loadCSV(const std::string& CSVStr)
    {
        std::stringstream ss(CSVStr);
        bool success = true;
        while (ss.good()){
            std::string templine;
            std::getline(ss, templine);
            std::stringstream lss(templine);
            if (templine.empty()){
                continue; 
            }
            while (lss.good()){
                std::string sflag;
                std::string svalue;
                std::getline(lss, sflag, ',');
                std::getline(lss, svalue, ',');
                if (svalue.empty()){
                    continue; 
                    success = false;
                }
                // check if options exists
                if (findOption(sflag)){
                    // parse the default data type
                    _optionValues[sflag] = parseValue(svalue.c_str(), _options[sflag].type()); 
                    log(LogLevel::INFO, std::string(sflag), "value is loaded from config");
                } else {
                    // parse string when the flag does not exist in the original configuration
                    _optionValues[sflag] = parseValue(svalue.c_str(), Value::DataType::STRING);
                    log(LogLevel::INFO, std::string(sflag), "value is not defined in config, parsed as a string value");
                }
            }
        }
        return success;
    }
 
#ifdef MINICONF_JSON_SUPPORT
    bool Config::getJSONValue(const picojson::value *v, const std::string& flag){
        bool success = true;
        if (findOption(flag)){
            Config::Option opt = _options[flag];
            if (opt.type() == Value::DataType::INT && v->is<double>()){
                _optionValues[flag] = static_cast<int>(v->get<double>());
            } else if (opt.type() == Value::DataType::NUMBER && v->is<double>()) {
                _optionValues[flag] = v->get<double>();
            } else if (opt.type() == Value::DataType::BOOL && v->is<bool>()) {
                _optionValues[flag] = v->get<bool>();
            } else if (opt.type() == Value::DataType::STRING && v->is<std::string>()) {
                _optionValues[flag] = v->get<std::string>();
            } else {
                log(LogLevel::WARNING, flag, "Unable to parse the option from config file, flag = " + flag);
                success = false;
            }
        }
        // stray options
        else 
        {
            if (v->is<double>()){
                _optionValues[flag] = v->get<double>();
            } 
            else if (v->is<bool>()) {
                _optionValues[flag] = v->get<bool>();
            } else if (v->is<std::string>()) {
                _optionValues[flag] = v->get<std::string>();
            } else {
                log(LogLevel::WARNING, flag, "Unable to parse the option from config file.");
                success = false;
            }
        }
        return success;
    }

    bool Config::parseJSON(const picojson::value *v, const std::string& flag){
        if (v->is<double>() || v->is<bool>() || v->is<std::string>()){
            return getJSONValue(v, flag);
        } else if (v->is<picojson::object>()){
            bool success = true;
            for (auto objItem: v->get<picojson::object>()){
                success = success && parseJSON(&(objItem.second), flag + (flag.empty() ? "" : ".") + objItem.first);
            }
            return success;
        } else if (v->is<picojson::array>()){
            for (auto arrayItem : v->get<picojson::array>()){
                // print array, not supported yet
            }
        } else {
            log(LogLevel::WARNING, flag, "Unable to parse JSON, abort, flag = " + flag);
            return false;
        }
        return false;
    }

    bool Config::loadJSON(const std::string& JSONStr)
    {
        picojson::value json;
        picojson::parse(json, JSONStr);
        return parseJSON(&json, "");
    }
#endif

}

// TODO: Stray arguments
// TODO: Support choice (value must be chosen form a list)
// TODO: Beautiful print, in help() and usage(), instead of printf()
// TODO: Support array
// TODO: Switch to JSON backend?

#endif // __MINICONF_H__

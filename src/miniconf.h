/*
 * miniconf.h
 *
 * A minimalist C++ configuration manager
 *
 * Author: Tsz-Ho Yu (thyu413@gmail.com)
 *
 */

/*
 * changelog:
 *
 * Version 1.0
 *     First workable version, polishing / documentation pending
 * Version 1.1
 *     Basic JSON export/import
 */

// TODO: remove option
// TODO: --cvs
// TODO: output cvs
// TODO: support choice (value must be chosen form a list)
// TODO: support array
// TODO: nested json

#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include "picojson.h"

/*
 * A flexible container for multiple data type
 */
class Value
{
public:

    // specifying data types that are held by this object. 
    // It can be an integer, a double-precision floating 
    // point number, a boolean or a string (char pointer).
    // An "unknown" type is also defined for empty container.
    enum class DataType {
        UNKNOWN,
        INT,
        NUMBER,
        BOOL,
        STRING
    };

    // Default constructors and assignments for Value
    Value();
    Value(const Value& other);
    Value(Value&& other);
    Value& operator=(const Value& other);
    Value& operator=(Value&& other);
    ~Value();

    // Value can be automatically cast as an integer
    // The getInt() function reads an integer explicitly
    explicit Value(const int& other);
    Value& operator=(const int& other);
    explicit operator int() const;
    int getInt() const;

    // Value can be automatically cast as a floating point number;
    // The getNumber() function reads a floating point number
    // explicitly.
    explicit Value(const double& other);
    Value& operator=(const double& other);
    explicit operator double() const;
    double getNumber() const;

    // Value can be automatically cast as a boolean;
    // The getBoolean() function reads a boolean explicitly
    explicit Value(const bool& other);
    Value& operator=(const bool& other);
    explicit operator bool() const;
    bool getBoolean() const;

    // Value can be automatically cast as a char array;
    // The getCharArray() functions read a char array explicitly
    explicit Value(const char* other);
    Value& operator=(const char* other);
    explicit operator char*() const;
    char* getCharArray() const;

    // Value can be automatically cast as a string;
    // The getString() functions read a string explicitly
    explicit Value(const std::string& other);
    Value& operator=(const std::string& other);
    explicit operator std::string() const;
    std::string getString() const;

    // print the JSON-compatible Value to a string
    std::string print();

    // print data type type to a string
    std::string printType();

    // get the data type of the Value
    DataType type();

    // check if the value is empty (unknown)
    bool isEmpty();
    
    // generate an unknown (empty) Value object
    static Value unknown();

private:

    // Move value data from a pointer
    Value& moveData(char*& src, const size_t size, const DataType type);
    
    // Copy value data from a pointer
    Value& copyData(const char* src, const size_t size, const DataType& type);
    
    // Clear allocated value data
    void clearData();
    
    // data type
    DataType _type;
    
    // size allocation (in bytes)
    size_t _size;
    
    // buffer for value data
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

    // Level of error/log message
    // INFO = normal log message
    // WARNING = something is wrong, but not fatal
    // ERROR = fatal error which causes undefined behavior
    // NONE = no message is generated, this is for the log() 
    //        function to suppress all log messages
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR,
        NONE
    };

    // Format of serialized config file
    // User can either serialize the current configurations, or 
    // write one config file manually
    enum class ExportFormat {
        JSON,
        CSV,
        YAML
    };

    // Option member class which describe the properties of a
    // configuration option.
    class Option;

    // Default constructor and destructor
    Config();
    ~Config();

    // The main parsing function for input command line
    bool parse(int argc, char** argv);

    // Create a new configuration option, uniquely identified by its flag
    Config::Option& option(const std::string& flag);
    // TODO to remove an option
    bool remove(const std::string& flag);
    // check if the option value is defined in the current configuration
    bool contains(const std::string& flag);
    // set current program description
    void description(const std::string& desc);

    // print the current configuration settings
    void print(FILE* fd = stdout);
    // print the current stack of log messages
    void log(FILE* fd = stdout);
    // set the logging level
    void log(const LogLevel logType);
    // display the log message
    void verbose(bool value);
    
    // access the configuration value
    // if the configuration value does not exist, 
    // an empty Value object is returned
    Value& operator[](const std::string& flag);

    // set the configuration settings via a config file
    void config(const std::string& configPath);

    // serialize the current configuration
    std::string serialize(const std::string& outFile = "", ExportFormat format = ExportFormat::JSON, bool pretty = true);

    // enable automatically generated help message (--help/-h)
    void enableHelp(bool enabled = true);

    // enable setting via external config file (--config/-cfg)
    void enableConfig(bool enabled = true);

    // print usage of this program's configuration options
    void usage(FILE* fd = stdout);

    // print help message 
    void help(FILE* fd = stdout);

private:

    // type of current argument token
    enum class TokenType {
        UNKNOWN,    // parsing error
        FLAG,       // a long flag, e.g. "--setting"
        SHORTFLAG,  // a short flag, e.g. "-s"
        VALUE       // value, e.g. 123, "hello", true
    };

    // set default values to configuration according to design
    void setDefaultValues();

    // check config format design
    LogLevel checkFormat();

    // validate user input after parsing / loading config file
    LogLevel validate();

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

    // load json config string
    void json(const std::string& JSONStr);

    // load yaml config string
    void yaml(const std::string& YAMLStr);

    // load csv config string
    void csv(const std::string& CSVStr);

    // internal function for adding log messages
    void log(LogLevel logType, const std::string& token, const std::string& msg);

    // this map stores configuration format design, e.g. flag, default values.
    std::map<std::string, Option> _options;

    // this map stores the values parsed form user input
    std::map<std::string, Value> _optionValues;

    // this is a stack of log messages
    std::vector<std::string> _log;

    bool _verbose;  // switch for verbose
    LogLevel _logLevel; // log level setting
    std::string _exeName; // program name stripped from command line
    std::string _description; // the program's description
    bool _autoHelp; // switch for auto help mesage
    bool _loadConfig; // switch for enable loading configuration
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

    // default constructor
    // should not be used by user
    Option();

    ~Option();

    // set the flag of an option
    Config::Option& flag(const std::string& flag);

    // set the short flag of an option
    Config::Option& shortflag(const std::string& shortflag);

    // set the description of an option
    Config::Option& description(const std::string& description);

    // set the default values of an option
    Config::Option& defaultValue(const Value& defaultValue);
    Config::Option& defaultValue(const int& defaultValue);
    Config::Option& defaultValue(const double& defaultValue);
    Config::Option& defaultValue(const bool& defaultValue);
    Config::Option& defaultValue(const char* defaultValue);
    Config::Option& defaultValue(const std::string& defaultValue);

    // decide if an option is required / optional
    Config::Option& required(const bool required);

    // print the flag of an option
    std::string flag();

    // print the shortflag of an option
    std::string shortflag();

    // print the description of an option
    std::string description();

    // print the default value of an option
    Value defaultValue();

    // determined if the option is required
    bool required();

    // get the data type 
    Value::DataType type();

private:
    std::string     _flag;          
    std::string     _shortflag;     
    std::string     _description;   
    Value           _defaultValue;
    bool            _required;
};

/*********************************************************************/
/*********************** IMPLEMENTATION BELOW ************************/
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
Config::Option::Option() : _flag(), _shortflag(), _description(), _defaultValue(Value::unknown()), _required(false)
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
    for (auto && val : _optionValues) {
        if (val.second.isEmpty()) {
            log(LogLevel::ERROR, val.first, "option contains invalid value");
            errorLv = worseLevel(errorLv, LogLevel::ERROR);
        }
    }
    for (auto && opt : _options) {
        if (!contains(opt.first)) {
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

    // * Load Config File
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

    // special case - if only two argument is provided, load 2nd argument as config
    if (argc == 2) {
        config(std::string(argv[1]));
    }

    // if contains help and auto-help is enabled, display help message
    if (contains("help") && _optionValues["help"].getBoolean() && _autoHelp) {
        help();
    }

    // remove help and config in optionValues
    // reserved words should not be displayed as normal config values
    if (contains("help")) _optionValues.erase("help");
    if (contains("config")) _optionValues.erase("config");

    // validate user inputs
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
        if (!o.defaultValue().isEmpty()) {
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
        option("config").shortflag("cfg").defaultValue("").description("Load the config file. Config format is determined by the file's extenion. If no file extension is found, default JSON loader is used").required(false);
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
        option("help").shortflag("h").defaultValue(false).description("Display the help message").required(false);
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

    printf("|------------------|------------|--------------------------------------------------|\n");
    printf("|       NAME       |    TYPE    |                     VALUE                        |\n");
    printf("|------------------|------------|--------------------------------------------------|\n");
    for (auto && v : _optionValues) {
        if (_options.find(v.first.c_str()) != _options.end()) {
            fprintf(fd, "| %-16s | %-10s | %-48s |\n", v.first.c_str(), v.second.printType().c_str(), v.second.print().c_str());
        } else {
            fprintf(fd, "| %-16s | %-10s | %-48s |\n", v.first.c_str(), (v.second.printType() + "*").c_str(), v.second.print().c_str());
        }
    }
    printf("|------------------|------------|--------------------------------------------------|\n");
    printf("\n");
}

std::string Config::serialize(const std::string& outFile, ExportFormat format, bool pretty)
{
    std::stringstream ss;
    std::string outStr;
    if (format == ExportFormat::JSON) {
        ss << "{";
        ss << ( pretty ? "\n" : "");
        for (auto opt = std::begin(_optionValues); opt != std::end(_optionValues); ++opt) {
            ss << ( pretty ? "    " : "");
            ss << "\"" << opt->first << "\"" << (pretty ? " : " : ":") << opt->second.print();
            ss << (opt != --std::end(_optionValues) ? "," : "");
            ss << ( pretty ? "\n" : "");
        }
        ss << "}";
        outStr = ss.str();
    }
    if (format == ExportFormat::CSV) {
        for (auto opt = std::begin(_optionValues); opt != std::end(_optionValues); ++opt) {
        }
    }
    if (format == ExportFormat::YAML) {

    }
    if (!outFile.empty()) {
        std::ofstream ofd(outFile);
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
    if (extension == "json" || extension == "JSON") {
        json(configContent);
        return;
    } else if (extension == "csv" || extension == "csv") {
        // csv(configContent)
        return;
    } else if (extension == "yaml" || extension == "YAML" || extension == "yml" || extension == "YML") {
        // yaml(configContent);
        return;
    } else {
        json(configContent);
    }
    return;
}

void Config::yaml(const std::string& YAMLStr)
{

}

void Config::csv(const std::string& CSVStr)
{

}

void Config::json(const std::string& JSONStr)
{
    picojson::value json;
    picojson::parse(json, JSONStr);
    picojson::object obj = json.get<picojson::object>();
    for (auto && o : obj) {
        std::string flag = o.first;
        picojson::value val = o.second;
        if (findOption(flag)) {
            Config::Option o = _options[flag];
            if (o.type() == Value::DataType::INT && val.is<double>()) {
                _optionValues[flag] = static_cast<int>(val.get<double>());
            }
            if (o.type() == Value::DataType::NUMBER && val.is<double>()) {
                _optionValues[flag] = val.get<double>();
            }
            if (o.type() == Value::DataType::BOOL && val.is<bool>()) {
                _optionValues[flag] = val.get<bool>();
            }
            if (o.type() == Value::DataType::STRING && val.is<std::string>()) {
                _optionValues[flag] = val.get<std::string>();
            }
        } else {
            if (val.is<double>()) {
                _optionValues[flag] = val.get<double>();
            }
            if (val.is<bool>()) {
                _optionValues[flag] = val.get<bool>();
            }
            if (val.is<std::string>()) {
                _optionValues[flag] = val.get<std::string>();
            }
        }
    }
}

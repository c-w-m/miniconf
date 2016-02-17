#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <vector>

// TODO: verbose
// TODO: Option Validation
// TODO: Value Validation
// TODO: program description
// TODO: Usage
// TODO: Help
// TODO: --json
// TODO: output json
// TODO: --cvs
// TODO: output cvs

class Value
{
public:

    enum class DataType
    {
        UNKNOWN,
        INT,
        NUMBER,
        BOOL,
        STRING
    };

    // Value-based functions 
    Value();
    Value(const Value& other);
    Value(Value&& other);
    Value& operator=(const Value& other);
    Value& operator=(Value&& other);
    ~Value();

    // int
    Value(const int& other);
    Value& operator=(const int& other);
    explicit operator int() const;
    int getInt() const;

    // number (floating point)
    Value(const double& other);
    Value& operator=(const double& other);
    explicit operator double() const;
    double getNumber() const;

    // bool
    Value(const bool& other);
    Value& operator=(const bool& other);
    explicit operator bool() const;
    bool getBoolean() const;

    // char array
    Value(const char* other);
    Value& operator=(const char* other);
    explicit operator char*() const;
    char* getCharArray() const;

    // string
    Value(const std::string& other);
    Value& operator=(const std::string& other);
    explicit operator std::string() const;
    std::string getString() const;

    // output functions
    std::string print();
    std::string printType();
    DataType type();
    bool isEmpty();
    static Value unknown();

private:

    Value& moveData(char*& src, const size_t size, const DataType type);
    Value& copyData(const char* src, const size_t size, const DataType& type);
    void clearData();

    DataType _type;
    size_t _size;
    char* _data;
};

class Config
{
public:
    class Option;
    Config();
    Config::Option& option(const std::string& flag);
    bool parse(int argc, char** argv);
    bool contains(const std::string& flag);
    void printOptions(FILE* fd = stdout); // TODO: to delete, replace by usage and help
    void print(FILE* fd = stdout);
    Value& operator[](const std::string& flag);
    void log(FILE* fd = stdout);

private:

    enum class TokenType
    {
        UNKNOWN,
        FLAG,
        SHORTFLAG,
        VALUE
    };

    enum class LogType
    {
        INFO,
        WARNING,
        ERROR
    };

    void setDefaultValues();
    TokenType getTokenType(const char* token);
    std::string translateShortflag(const std::string& shortflag);
    Option* getOption(const char* token, Config::TokenType tokenType);
    Value parseValue(const char* token, Value::DataType dataType);


    void log(LogType logType, const char* token, const char* msg);

    std::map<std::string, Option> _options;
    std::map<std::string, Value> _optionValues;
    std::vector<std::string> _log;
};

class Config::Option
{
public:
    Option();
    Config::Option& flag(const std::string& flag);
    Config::Option& shortflag(const std::string& shortflag);
    Config::Option& description(const std::string& description);
    Config::Option& defaultValue(const Value& defaultValue);
    Config::Option& defaultValue(const int& defaultValue);
    Config::Option& defaultValue(const double& defaultValue);
    Config::Option& defaultValue(const bool& defaultValue);
    Config::Option& defaultValue(const char* defaultValue);
    Config::Option& defaultValue(const std::string& defaultValue);
    Config::Option& required(const bool required);

    std::string flag();
    std::string shortflag();
    std::string description();
    Value defaultValue();
    bool required();
    Value::DataType type();
private:
    std::string     _flag;
    std::string     _shortflag;
    std::string     _description;
    Value           _defaultValue;
    bool            _required;
};

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
    copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::NUMBER);
}

Value& Value::operator=(const bool& other)
{
    clearData();
    return copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::NUMBER);
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
            outStr = std::string(getCharArray());
            break;
        default: break;
    }
    return outStr;
}

Value::DataType Value::type()
{
    return _type;
}

bool Value::isEmpty()
{
    return (_data == nullptr || _type == DataType::UNKNOWN);
}

Value Value::unknown()
{
    return Value();
}

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
        default: break;
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

Value& Value::copyData(const char* src, const size_t size, const DataType& type)
{
    char* newData = new char[size];
    memcpy(newData, src, size);
    return moveData(newData, size, type);
}

void Value::clearData()
{
    if (_size != 0 && _data != nullptr) {
        delete _data;
        _data = nullptr;
    }
}

Config::Option::Option() : _flag(), _shortflag(), _description(), _defaultValue(Value::unknown()), _required(false) {}

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

Config::Config()
{

}

Config::Option& Config::option(const std::string& flag)
{
    _options.insert(std::make_pair(flag, Config::Option().flag(flag)));
    return _options[flag];
}

void Config::setDefaultValues()
{
    for (auto&& o : _options) {
        _optionValues[o.first] = o.second.defaultValue();
    }
}


void Config::log(FILE* fd)
{
    fprintf(fd, "\n\n[[[  %s  ]]]\n\n", "PARSE LOG");
    for (auto&& logline : _log) {
        fprintf(fd, "%s\n", logline.c_str());
    }
}

void Config::log(Config::LogType logType, const char* token, const char* msg)
{
    const int tagWidth = 16;
    char tag[tagWidth + 1];
    char format[tagWidth + 1];
    std::string logString = "";
    snprintf(format, tagWidth, "<<<%%%ds>>>", tagWidth - 7);
    switch (logType) {
        case Config::LogType::INFO:
            snprintf(tag, tagWidth, format, "INFO");
            break;
        case Config::LogType::WARNING:
            snprintf(tag, tagWidth, format, "WARNING");
            break;
        case Config::LogType::ERROR:
            snprintf(tag, tagWidth, format, "ERROR");
            break;
        default:
            break;
    }
    logString = std::string(tag) + " Input \"" + std::string(token) + "\" : " + msg;
    _log.emplace_back(logString);
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
        double testval;
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

Config::Option* Config::getOption(const char* token, Config::TokenType tokenType)
{
    std::string flag = "";
    if (tokenType == TokenType::FLAG) {
        flag = std::string(token + 2);
    }
    else if (tokenType == TokenType::SHORTFLAG) {
        flag = translateShortflag(std::string(token + 1));
    }
    if (flag.empty()) {
        return nullptr;
    }
    auto found = _options.find(flag);
    if (found != _options.end()) {
        return &(found->second);
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

bool Config::parse(int argc, char **argv)
{
    // debug
    /*
    for (int i = 0; i < argc; ++i) {
        printf("ARGV[%2d] %s\n", i, argv[i]);
    }
    */

    Config::Option wildcard;
    wildcard.defaultValue("");  // string argument by default

    // validateOptions()

    setDefaultValues();

    // validateArguments()
    std::string currentFlag = "";
    Option* currentOption = nullptr;
    for (int i = 1; i < argc; ++i) {
        TokenType currentTokenType = getTokenType(argv[i]);
        if (currentTokenType == TokenType::UNKNOWN) {
            // log(log::error, argv[i], "unknown input");
        }
        else if (currentTokenType == TokenType::FLAG || currentTokenType == TokenType::SHORTFLAG) {
            currentOption = getOption(argv[i], currentTokenType);
            if (!currentOption) {
                log(LogType::WARNING, argv[i], "unrecognized flag");
                if (currentTokenType == TokenType::FLAG){
                    wildcard.flag(std::string(argv[i] + 2));
                    currentOption = &wildcard;
                }
            }
            // special case - if the option type is bool, set to true by default
            if (currentOption && currentOption->type() == Value::DataType::BOOL) {
                _optionValues[currentOption->flag()] = true;
            }
        }
        else if (currentTokenType == TokenType::VALUE) {
            if (currentOption) {
                // current Option
                Value newValue = parseValue(argv[i], currentOption->type());
                if (newValue.isEmpty()){
                    log(LogType::WARNING, argv[i], "unvalid value type is provided");
                }else{
                    _optionValues[currentOption->flag()] = parseValue(argv[i], currentOption->type());
                    log(LogType::INFO, argv[i], "value parsed successfully");
                }
                // reset current option -> ready for a new flag
                currentOption = nullptr;
            }
            else {
                // stray arguments
                log(LogType::WARNING, argv[i], "unassociated argument");
            }
        }
    }
    return true;
}

void Config::printOptions(FILE* fd)
{
    for (auto&& o : _options) {
        fprintf(fd, "[FLAG: %10s SHORT: %3s DESC: %10s Default: %10s Required: %5s Type: %8s]\n",
                o.second.flag().c_str(),
                o.second.shortflag().c_str(),
                o.second.description().c_str(),
                o.second.defaultValue().print().c_str(),
                o.second.required() ? "true" : "false",
                o.second.defaultValue().printType().c_str()
                );
    }
}

bool Config::contains(const std::string& flag)
{
    return _optionValues.find(flag) != _optionValues.end();
}

Value& Config::operator[](const std::string& flag)
{
    return _optionValues[flag];
}

void Config::print(FILE* fd){
    fprintf(fd, "\n\n[[[  %s  ]]]\n\n", "CONFIGURATION");

    printf("|------------------|------------|--------------------------------------------------|\n");
    printf("|       NAME       |    TYPE    |                     VALUE                        |\n");
    printf("|------------------|------------|--------------------------------------------------|\n");
    for (auto&& v: _optionValues){
        if (_options.find(v.first.c_str()) != _options.end()){
            fprintf(fd, "| %-16s | %-10s | %-48s |\n", v.first.c_str(), v.second.printType().c_str(), v.second.print().c_str());
        } else{
            fprintf(fd, "| %-16s | %-10s | %-48s |\n", v.first.c_str(), (v.second.printType() + "*").c_str(), v.second.print().c_str());
        }
    }
    printf("|------------------|------------|--------------------------------------------------|\n");
}

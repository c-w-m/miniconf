#include <cstring>
#include <string>
#include <sstream>

class Value {
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
        Value(const Value& other);
        Value(Value&& other);
        Value& operator=(const Value& other);
        Value& operator=(Value&& other);

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
        DataType type();
        bool isEmpty();
        static Value unknown();

    private:
        
        Value();
        Value& moveData(char*& src, const size_t size, const DataType type);
        Value& copyData(const char* src, const size_t size, const DataType& type);

        DataType _type;
        size_t _size;
        char* _data;
};

class Config
{
    public:
        class Option;
    private:
};

class Config::Option
{
    public:
        Option();
        Config::Option& flag(const std::string& flag);
        Config::Option& shortflag(const std::string& shortflag);
        Config::Option& description(const std::string& description);
        Config::Option& defaultValue(const Value& defaultValue);
        Config::Option& required(const bool required);
        std::string description();
    private:
        std::string     _flag;
        std::string     _shortflag;
        std::string     _description;
        Value           _defaultValue;
        bool            _required;
};

Value::Value(): _type(DataType::UNKNOWN), _size(0), _data(nullptr) 
{ 
}

Value::Value(const Value& other): Value()
{
    copyData(other._data, other._size, other._type);
}

Value::Value(Value&& other): Value()
{
    moveData(other._data, other._size, other._type);
}
        

Value& Value::operator=(const Value& other)
{
    return copyData(other._data, other._size, other._type);
}

Value& Value::operator=(Value&& other)
{
    return moveData(other._data, other._size, other._type);
}
        
//  int
Value::Value(const int& other): Value()
{   
    copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
}

Value& Value::operator=(const int& other)
{
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
Value::Value(const double& other): Value()
{   
    copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
}

Value& Value::operator=(const double& other)
{
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
Value::Value(const bool& other): Value()
{   
    copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::NUMBER);
}

Value& Value::operator=(const bool& other)
{
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
Value::Value(const char* other): Value()
{   
    copyData(other, strlen(other) + 1, DataType::STRING);
}

Value& Value::operator=(const char* other)
{
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
Value::Value(const std::string& other): Value()
{   
    copyData(other.c_str(), other.size() + 1, DataType::STRING);
}

Value& Value::operator=(const std::string& other)
{
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
    int slen = 31;
    char tempStr[slen + 1];
    std::string outStr;
    switch (_type)
    {
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
            snprintf(tempStr, slen, "%s", getBoolean()? "true" : "false");
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

// move and copy function
Value& Value::moveData(char*& src, const size_t size, const DataType type)
{
    _type = type;
    _size = size;
    if (_data != nullptr){
        delete _data;
        _data = nullptr;
    }
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

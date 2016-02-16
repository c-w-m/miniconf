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
        explicit Value(const Value& other);
        explicit Value(Value&& other);
        Value& operator=(const Value& other);
        Value& operator=(Value&& other);

        // int
        explicit Value(const int& other);
        Value& operator=(const int& other);
        explicit operator int();
        int getInt();

        // number (floating point)
        explicit Value(const double& other);
        Value& operator=(const double& other);
        explicit operator double();
        double getNumber();

        // bool
        explicit Value(const bool& other);
        Value& operator=(const bool& other);
        explicit operator bool();
        bool getBoolean();
        
        // char array
        explicit Value(const char* other);
        Value& operator=(const char* other);
        explicit operator char*();
        char* getCharArray();

        // string
        explicit Value(const std::string& other);
        Value& operator=(const std::string& other);
        explicit operator std::string();
        std::string getString();

        // output functions
        std::string print();
        DataType type();
        bool isEmpty();

    private:
        
        Value();
        Value& moveData(char*& src, const size_t size, const DataType type);
        Value& copyData(const char* src, const size_t size, const DataType& type);

        DataType _type;
        size_t _size;
        char* _data;
};

Value::Value(): _type(DataType::UNKNOWN), _size(0), _data(nullptr) 
{ 
}

Value::Value(const Value& other): Value()
{
    copyData(other._data, other._size, other._type);
    printf("LValue Value Constructor\n");
}

Value::Value(Value&& other): Value()
{
    moveData(other._data, other._size, other._type);
    printf("RValue Value Constructor\n");
}
        

Value& Value::operator=(const Value& other)
{
    printf("LValue Value Assignment\n");
    return copyData(other._data, other._size, other._type);
}

Value& Value::operator=(Value&& other)
{
    printf("RValue Value Assignment\n");
    return moveData(other._data, other._size, other._type);
}
        
//  int
Value::Value(const int& other): Value()
{   
    printf("Int ctor\n");
    copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
}

Value& Value::operator=(const int& other)
{
    printf("Int assignment\n");
    return copyData(reinterpret_cast<const char*>(&other), sizeof(int), DataType::INT);
}

Value::operator int()
{
    return *reinterpret_cast<int*>(_data);
}

int Value::getInt()
{
    return *reinterpret_cast<int*>(_data);
}

//  number (floating point)
Value::Value(const double& other): Value()
{   
    printf("float ctor\n");
    copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
}

Value& Value::operator=(const double& other)
{
    printf("float assignment\n");
    return copyData(reinterpret_cast<const char*>(&other), sizeof(double), DataType::NUMBER);
}

Value::operator double()
{
    return *reinterpret_cast<double*>(_data);
}

double Value::getNumber()
{
    return *reinterpret_cast<double*>(_data);
}


//  bool
Value::Value(const bool& other): Value()
{   
    printf("bool ctor\n");
    copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::NUMBER);
}

Value& Value::operator=(const bool& other)
{
    printf("bool assignment\n");
    return copyData(reinterpret_cast<const char*>(&other), sizeof(bool), DataType::NUMBER);
}

Value::operator bool()
{
    return *reinterpret_cast<bool*>(_data);
}

bool Value::getBoolean()
{
    return *reinterpret_cast<bool*>(_data);
}

//  char array
Value::Value(const char* other): Value()
{   
    printf("char* ctor\n");
    copyData(other, strlen(other) + 1, DataType::STRING);
}

Value& Value::operator=(const char* other)
{
    printf("char* assignment\n");
    return copyData(other, strlen(other) + 1, DataType::STRING);
}

Value::operator char*()
{
    return reinterpret_cast<char*>(_data);
}

char* Value::getCharArray()
{
    return reinterpret_cast<char*>(_data);
}

//  std::string
Value::Value(const std::string& other): Value()
{   
    printf("std::string ctor\n");
    copyData(other.c_str(), other.size() + 1, DataType::STRING);
}

Value& Value::operator=(const std::string& other)
{
    printf("std::string assignment\n");
    return copyData(other.c_str(), other.size() + 1, DataType::STRING);
}

Value::operator std::string()
{
    return std::string(reinterpret_cast<char*>(_data));
}

std::string Value::getString()
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

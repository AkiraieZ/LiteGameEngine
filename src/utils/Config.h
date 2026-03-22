#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <type_traits>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <limits>

namespace LGE {

class ConfigValue {
public:
    enum class Type {
        Null,
        Bool,
        Int,
        Float,
        Double,
        String,
        Array,
        Object
    };

    ConfigValue() : m_type(Type::Null) {}

    ConfigValue(bool value) : m_type(Type::Bool) { m_data.boolVal = value; }

    ConfigValue(int value) : m_type(Type::Int) { m_data.intVal = value; }

    ConfigValue(float value) : m_type(Type::Float) { m_data.floatVal = value; }

    ConfigValue(double value) : m_type(Type::Double) { m_data.doubleVal = value; }

    ConfigValue(const char* value) : m_type(Type::String) {
        new (&m_data.stringVal) std::string(value);
    }

    ConfigValue(const std::string& value) : m_type(Type::String) {
        new (&m_data.stringVal) std::string(value);
    }

    ConfigValue(std::string&& value) : m_type(Type::String) {
        new (&m_data.stringVal) std::string(std::move(value));
    }

    ConfigValue(const std::vector<ConfigValue>& value) : m_type(Type::Array) {
        new (&m_data.arrayVal) std::vector<ConfigValue>(value);
    }

    ConfigValue(std::vector<ConfigValue>&& value) : m_type(Type::Array) {
        new (&m_data.arrayVal) std::vector<ConfigValue>(std::move(value));
    }

    ConfigValue(const std::map<std::string, ConfigValue>& value) : m_type(Type::Object) {
        new (&m_data.objectVal) std::map<std::string, ConfigValue>(value);
    }

    ConfigValue(std::map<std::string, ConfigValue>&& value) : m_type(Type::Object) {
        new (&m_data.objectVal) std::map<std::string, ConfigValue>(std::move(value));
    }

    ConfigValue(const ConfigValue& other) : m_type(other.m_type) {
        copyFrom(other);
    }

    ConfigValue(ConfigValue&& other) noexcept : m_type(other.m_type) {
        moveFrom(std::move(other));
    }

    ~ConfigValue() {
        destroy();
    }

    ConfigValue& operator=(const ConfigValue& other) {
        if (this != &other) {
            destroy();
            m_type = other.m_type;
            copyFrom(other);
        }
        return *this;
    }

    ConfigValue& operator=(ConfigValue&& other) noexcept {
        if (this != &other) {
            destroy();
            m_type = other.m_type;
            moveFrom(std::move(other));
        }
        return *this;
    }

    Type getType() const { return m_type; }

    bool isNull() const { return m_type == Type::Null; }
    bool isBool() const { return m_type == Type::Bool; }
    bool isInt() const { return m_type == Type::Int; }
    bool isFloat() const { return m_type == Type::Float; }
    bool isDouble() const { return m_type == Type::Double; }
    bool isNumber() const { return isInt() || isFloat() || isDouble(); }
    bool isString() const { return m_type == Type::String; }
    bool isArray() const { return m_type == Type::Array; }
    bool isObject() const { return m_type == Type::Object; }

    bool asBool() const {
        if (!isBool()) throw std::runtime_error("ConfigValue is not a bool");
        return m_data.boolVal;
    }

    bool asBool(bool defaultValue) const {
        if (!isBool()) return defaultValue;
        return m_data.boolVal;
    }

    int asInt() const {
        if (isInt()) return m_data.intVal;
        if (isFloat()) return static_cast<int>(m_data.floatVal);
        if (isDouble()) return static_cast<int>(m_data.doubleVal);
        throw std::runtime_error("ConfigValue is not a number");
    }

    int asInt(int defaultValue) const {
        if (isInt()) return m_data.intVal;
        if (isFloat()) return static_cast<int>(m_data.floatVal);
        if (isDouble()) return static_cast<int>(m_data.doubleVal);
        return defaultValue;
    }

    float asFloat() const {
        if (isFloat()) return m_data.floatVal;
        if (isInt()) return static_cast<float>(m_data.intVal);
        if (isDouble()) return static_cast<float>(m_data.doubleVal);
        throw std::runtime_error("ConfigValue is not a number");
    }

    float asFloat(float defaultValue) const {
        if (isFloat()) return m_data.floatVal;
        if (isInt()) return static_cast<float>(m_data.intVal);
        if (isDouble()) return static_cast<float>(m_data.doubleVal);
        return defaultValue;
    }

    double asDouble() const {
        if (isDouble()) return m_data.doubleVal;
        if (isInt()) return static_cast<double>(m_data.intVal);
        if (isFloat()) return static_cast<double>(m_data.floatVal);
        throw std::runtime_error("ConfigValue is not a number");
    }

    double asDouble(double defaultValue) const {
        if (isDouble()) return m_data.doubleVal;
        if (isInt()) return static_cast<double>(m_data.intVal);
        if (isFloat()) return static_cast<double>(m_data.floatVal);
        return defaultValue;
    }

    const std::string& asString() const {
        if (!isString()) throw std::runtime_error("ConfigValue is not a string");
        return m_data.stringVal;
    }

    const std::string& asString(const std::string& defaultValue) const {
        if (!isString()) return defaultValue;
        return m_data.stringVal;
    }

    const std::vector<ConfigValue>& asArray() const {
        if (!isArray()) throw std::runtime_error("ConfigValue is not an array");
        return m_data.arrayVal;
    }

    std::vector<ConfigValue>& asArray() {
        if (!isArray()) throw std::runtime_error("ConfigValue is not an array");
        return m_data.arrayVal;
    }

    const std::map<std::string, ConfigValue>& asObject() const {
        if (!isObject()) throw std::runtime_error("ConfigValue is not an object");
        return m_data.objectVal;
    }

    std::map<std::string, ConfigValue>& asObject() {
        if (!isObject()) throw std::runtime_error("ConfigValue is not an object");
        return m_data.objectVal;
    }

    size_t size() const {
        if (isArray()) return m_data.arrayVal.size();
        if (isObject()) return m_data.objectVal.size();
        if (isString()) return m_data.stringVal.size();
        return 0;
    }

    bool has(const std::string& key) const {
        if (!isObject()) return false;
        return m_data.objectVal.find(key) != m_data.objectVal.end();
    }

    const ConfigValue& operator[](const std::string& key) const {
        if (!isObject()) throw std::runtime_error("ConfigValue is not an object");
        return m_data.objectVal.at(key);
    }

    ConfigValue& operator[](const std::string& key) {
        if (!isObject()) {
            destroy();
            m_type = Type::Object;
            new (&m_data.objectVal) std::map<std::string, ConfigValue>();
        }
        return m_data.objectVal[key];
    }

    const ConfigValue& operator[](size_t index) const {
        if (!isArray()) throw std::runtime_error("ConfigValue is not an array");
        return m_data.arrayVal.at(index);
    }

    ConfigValue& operator[](size_t index) {
        if (!isArray()) {
            destroy();
            m_type = Type::Array;
            new (&m_data.arrayVal) std::vector<ConfigValue>();
        }
        return m_data.arrayVal[index];
    }

    void set(const std::string& key, const ConfigValue& value) {
        (*this)[key] = value;
    }

    void set(const std::string& key, ConfigValue&& value) {
        (*this)[key] = std::move(value);
    }

    void push(const ConfigValue& value) {
        if (!isArray()) {
            destroy();
            m_type = Type::Array;
            new (&m_data.arrayVal) std::vector<ConfigValue>();
        }
        m_data.arrayVal.push_back(value);
    }

    void push(ConfigValue&& value) {
        if (!isArray()) {
            destroy();
            m_type = Type::Array;
            new (&m_data.arrayVal) std::vector<ConfigValue>();
        }
        m_data.arrayVal.push_back(std::move(value));
    }

    std::string toString(int indent = 0) const {
        std::ostringstream oss;
        toString(oss, indent, 0);
        return oss.str();
    }

    static ConfigValue parse(const std::string& str) {
        size_t pos = 0;
        return parseValue(str, pos);
    }

    static ConfigValue parseFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Could not open config file: " + filename);
        }
        std::string content((std::istreambuf_iterator<char>(file)), 
                            std::istreambuf_iterator<char>());
        return parse(content);
    }

    bool saveToFile(const std::string& filename, int indent = 2) const {
        std::ofstream file(filename);
        if (!file) return false;
        file << toString(indent);
        return file.good();
    }

private:
    Type m_type;
    union Data {
        bool boolVal;
        int intVal;
        float floatVal;
        double doubleVal;
        std::string stringVal;
        std::vector<ConfigValue> arrayVal;
        std::map<std::string, ConfigValue> objectVal;

        Data() {}
        ~Data() {}
    } m_data;

    void destroy() {
        switch (m_type) {
            case Type::String:
                m_data.stringVal.~basic_string();
                break;
            case Type::Array:
                m_data.arrayVal.~vector();
                break;
            case Type::Object:
                m_data.objectVal.~map();
                break;
            default:
                break;
        }
    }

    void copyFrom(const ConfigValue& other) {
        switch (other.m_type) {
            case Type::Bool:
                m_data.boolVal = other.m_data.boolVal;
                break;
            case Type::Int:
                m_data.intVal = other.m_data.intVal;
                break;
            case Type::Float:
                m_data.floatVal = other.m_data.floatVal;
                break;
            case Type::Double:
                m_data.doubleVal = other.m_data.doubleVal;
                break;
            case Type::String:
                new (&m_data.stringVal) std::string(other.m_data.stringVal);
                break;
            case Type::Array:
                new (&m_data.arrayVal) std::vector<ConfigValue>(other.m_data.arrayVal);
                break;
            case Type::Object:
                new (&m_data.objectVal) std::map<std::string, ConfigValue>(other.m_data.objectVal);
                break;
            default:
                break;
        }
    }

    void moveFrom(ConfigValue&& other) {
        switch (other.m_type) {
            case Type::Bool:
                m_data.boolVal = other.m_data.boolVal;
                break;
            case Type::Int:
                m_data.intVal = other.m_data.intVal;
                break;
            case Type::Float:
                m_data.floatVal = other.m_data.floatVal;
                break;
            case Type::Double:
                m_data.doubleVal = other.m_data.doubleVal;
                break;
            case Type::String:
                new (&m_data.stringVal) std::string(std::move(other.m_data.stringVal));
                break;
            case Type::Array:
                new (&m_data.arrayVal) std::vector<ConfigValue>(std::move(other.m_data.arrayVal));
                break;
            case Type::Object:
                new (&m_data.objectVal) std::map<std::string, ConfigValue>(std::move(other.m_data.objectVal));
                break;
            default:
                break;
        }
        other.m_type = Type::Null;
    }

    void toString(std::ostringstream& oss, int indent, int level) const {
        std::string indentStr(indent * level, ' ');
        std::string indentStr2(indent * (level + 1), ' ');

        switch (m_type) {
            case Type::Null:
                oss << "null";
                break;
            case Type::Bool:
                oss << (m_data.boolVal ? "true" : "false");
                break;
            case Type::Int:
                oss << m_data.intVal;
                break;
            case Type::Float:
                oss << m_data.floatVal;
                break;
            case Type::Double:
                oss << m_data.doubleVal;
                break;
            case Type::String:
                oss << '"' << escapeString(m_data.stringVal) << '"';
                break;
            case Type::Array:
                oss << '[';
                if (!m_data.arrayVal.empty()) {
                    oss << '\n';
                    for (size_t i = 0; i < m_data.arrayVal.size(); i++) {
                        oss << indentStr2;
                        m_data.arrayVal[i].toString(oss, indent, level + 1);
                        if (i != m_data.arrayVal.size() - 1) oss << ',';
                        oss << '\n';
                    }
                    oss << indentStr;
                }
                oss << ']';
                break;
            case Type::Object:
                oss << '{';
                if (!m_data.objectVal.empty()) {
                    oss << '\n';
                    size_t i = 0;
                    for (const auto& [key, value] : m_data.objectVal) {
                        oss << indentStr2 << '"' << escapeString(key) << "\": ";
                        value.toString(oss, indent, level + 1);
                        if (i != m_data.objectVal.size() - 1) oss << ',';
                        oss << '\n';
                        i++;
                    }
                    oss << indentStr;
                }
                oss << '}';
                break;
        }
    }

    static std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
            pos++;
        }
    }

    static ConfigValue parseValue(const std::string& str, size_t& pos) {
        skipWhitespace(str, pos);
        if (pos >= str.size()) return ConfigValue();

        char c = str[pos];
        if (c == 'n') return parseNull(str, pos);
        if (c == 't' || c == 'f') return parseBool(str, pos);
        if (c == '"') return parseString(str, pos);
        if (c == '[') return parseArray(str, pos);
        if (c == '{') return parseObject(str, pos);
        if (c == '-' || c == '+' || std::isdigit(static_cast<unsigned char>(c))) {
            return parseNumber(str, pos);
        }
        throw std::runtime_error("Unexpected character in config");
    }

    static ConfigValue parseNull(const std::string& str, size_t& pos) {
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            return ConfigValue();
        }
        throw std::runtime_error("Invalid null value");
    }

    static ConfigValue parseBool(const std::string& str, size_t& pos) {
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return ConfigValue(true);
        }
        if (str.substr(pos, 5) == "false") {
            pos += 5;
            return ConfigValue(false);
        }
        throw std::runtime_error("Invalid boolean value");
    }

    static ConfigValue parseString(const std::string& str, size_t& pos) {
        pos++;
        std::string result;
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++;
                switch (str[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[pos]; break;
                }
            } else {
                result += str[pos];
            }
            pos++;
        }
        pos++;
        return ConfigValue(result);
    }

    static ConfigValue parseNumber(const std::string& str, size_t& pos) {
        size_t start = pos;
        bool isDouble = false;
        bool isFloat = false;

        if (str[pos] == '-' || str[pos] == '+') pos++;
        while (pos < str.size() && std::isdigit(static_cast<unsigned char>(str[pos]))) {
            pos++;
        }
        if (pos < str.size() && str[pos] == '.') {
            isDouble = true;
            pos++;
            while (pos < str.size() && std::isdigit(static_cast<unsigned char>(str[pos]))) {
                pos++;
            }
        }
        if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
            isDouble = true;
            pos++;
            if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) pos++;
            while (pos < str.size() && std::isdigit(static_cast<unsigned char>(str[pos]))) {
                pos++;
            }
        }
        if (pos < str.size() && (str[pos] == 'f' || str[pos] == 'F')) {
            isFloat = true;
            isDouble = false;
            pos++;
        }

        std::string numStr = str.substr(start, pos - start);
        if (isFloat) {
            return ConfigValue(std::stof(numStr));
        } else if (isDouble) {
            return ConfigValue(std::stod(numStr));
        } else {
            return ConfigValue(std::stoi(numStr));
        }
    }

    static ConfigValue parseArray(const std::string& str, size_t& pos) {
        pos++;
        ConfigValue arr;
        arr.m_type = Type::Array;
        new (&arr.m_data.arrayVal) std::vector<ConfigValue>();

        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == ']') {
            pos++;
            return arr;
        }

        while (true) {
            arr.m_data.arrayVal.push_back(parseValue(str, pos));
            skipWhitespace(str, pos);
            if (pos >= str.size()) break;
            if (str[pos] == ']') {
                pos++;
                break;
            }
            if (str[pos] == ',') {
                pos++;
                skipWhitespace(str, pos);
            }
        }
        return arr;
    }

    static ConfigValue parseObject(const std::string& str, size_t& pos) {
        pos++;
        ConfigValue obj;
        obj.m_type = Type::Object;
        new (&obj.m_data.objectVal) std::map<std::string, ConfigValue>();

        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == '}') {
            pos++;
            return obj;
        }

        while (true) {
            skipWhitespace(str, pos);
            ConfigValue key = parseString(str, pos);
            skipWhitespace(str, pos);
            if (pos >= str.size() || str[pos] != ':') {
                throw std::runtime_error("Expected ':' in object");
            }
            pos++;
            obj.m_data.objectVal[key.asString()] = parseValue(str, pos);
            skipWhitespace(str, pos);
            if (pos >= str.size()) break;
            if (str[pos] == '}') {
                pos++;
                break;
            }
            if (str[pos] == ',') {
                pos++;
                skipWhitespace(str, pos);
            }
        }
        return obj;
    }
};

class Config {
public:
    Config() = default;

    explicit Config(const std::string& filename) {
        load(filename);
    }

    bool load(const std::string& filename) {
        try {
            m_root = ConfigValue::parseFile(filename);
            m_filename = filename;
            return true;
        } catch (...) {
            return false;
        }
    }

    bool save(const std::string& filename = "") const {
        std::string file = filename.empty() ? m_filename : filename;
        if (file.empty()) return false;
        return m_root.saveToFile(file);
    }

    bool has(const std::string& key) const {
        return m_root.has(key);
    }

    bool hasPath(const std::string& path) const {
        std::vector<std::string> parts = splitPath(path);
        const ConfigValue* current = &m_root;
        for (const auto& part : parts) {
            if (!current->has(part)) return false;
            current = &(*current)[part];
        }
        return true;
    }

    ConfigValue& get(const std::string& key) {
        return m_root[key];
    }

    const ConfigValue& get(const std::string& key) const {
        return m_root[key];
    }

    ConfigValue& getPath(const std::string& path) {
        std::vector<std::string> parts = splitPath(path);
        ConfigValue* current = &m_root;
        for (const auto& part : parts) {
            current = &(*current)[part];
        }
        return *current;
    }

    const ConfigValue& getPath(const std::string& path) const {
        std::vector<std::string> parts = splitPath(path);
        const ConfigValue* current = &m_root;
        for (const auto& part : parts) {
            current = &(*current)[part];
        }
        return *current;
    }

    void set(const std::string& key, const ConfigValue& value) {
        m_root[key] = value;
    }

    void set(const std::string& key, ConfigValue&& value) {
        m_root[key] = std::move(value);
    }

    void setPath(const std::string& path, const ConfigValue& value) {
        std::vector<std::string> parts = splitPath(path);
        ConfigValue* current = &m_root;
        for (size_t i = 0; i < parts.size() - 1; i++) {
            current = &(*current)[parts[i]];
        }
        (*current)[parts.back()] = value;
    }

    void setPath(const std::string& path, ConfigValue&& value) {
        std::vector<std::string> parts = splitPath(path);
        ConfigValue* current = &m_root;
        for (size_t i = 0; i < parts.size() - 1; i++) {
            current = &(*current)[parts[i]];
        }
        (*current)[parts.back()] = std::move(value);
    }

    bool getBool(const std::string& key, bool defaultValue = false) const {
        if (!m_root.has(key)) return defaultValue;
        return m_root[key].asBool(defaultValue);
    }

    int getInt(const std::string& key, int defaultValue = 0) const {
        if (!m_root.has(key)) return defaultValue;
        return m_root[key].asInt(defaultValue);
    }

    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        if (!m_root.has(key)) return defaultValue;
        return m_root[key].asFloat(defaultValue);
    }

    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        if (!m_root.has(key)) return defaultValue;
        return m_root[key].asDouble(defaultValue);
    }

    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        if (!m_root.has(key)) return defaultValue;
        return m_root[key].asString(defaultValue);
    }

    bool getBoolPath(const std::string& path, bool defaultValue = false) const {
        if (!hasPath(path)) return defaultValue;
        return getPath(path).asBool(defaultValue);
    }

    int getIntPath(const std::string& path, int defaultValue = 0) const {
        if (!hasPath(path)) return defaultValue;
        return getPath(path).asInt(defaultValue);
    }

    float getFloatPath(const std::string& path, float defaultValue = 0.0f) const {
        if (!hasPath(path)) return defaultValue;
        return getPath(path).asFloat(defaultValue);
    }

    double getDoublePath(const std::string& path, double defaultValue = 0.0) const {
        if (!hasPath(path)) return defaultValue;
        return getPath(path).asDouble(defaultValue);
    }

    std::string getStringPath(const std::string& path, const std::string& defaultValue = "") const {
        if (!hasPath(path)) return defaultValue;
        return getPath(path).asString(defaultValue);
    }

    ConfigValue& root() { return m_root; }
    const ConfigValue& root() const { return m_root; }

    std::string toString(int indent = 2) const {
        return m_root.toString(indent);
    }

    void clear() {
        m_root = ConfigValue();
    }

private:
    std::string m_filename;
    ConfigValue m_root;

    static std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> result;
        std::stringstream ss(path);
        std::string part;
        while (std::getline(ss, part, '.')) {
            if (!part.empty()) {
                result.push_back(part);
            }
        }
        return result;
    }
};

}

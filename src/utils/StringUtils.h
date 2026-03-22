#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <functional>
#include <iomanip>

namespace LGE {

namespace StringUtils {

bool isEmpty(const std::string& str);
bool isBlank(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
bool contains(const std::string& str, const std::string& substr);
bool contains(const std::string& str, char c);
bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
bool compare(const std::string& str1, const std::string& str2, bool caseSensitive = true);

std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
std::string capitalize(const std::string& str);
std::string decapitalize(const std::string& str);
std::string reverse(const std::string& str);

std::string trim(const std::string& str);
std::string trimStart(const std::string& str);
std::string trimEnd(const std::string& str);
std::string trim(const std::string& str, const std::string& chars);
std::string trimStart(const std::string& str, const std::string& chars);
std::string trimEnd(const std::string& str, const std::string& chars);

std::string padLeft(const std::string& str, size_t width, char fillChar = ' ');
std::string padRight(const std::string& str, size_t width, char fillChar = ' ');
std::string center(const std::string& str, size_t width, char fillChar = ' ');

std::string repeat(const std::string& str, int count);
std::string repeat(char c, int count);

std::string replace(const std::string& str, const std::string& from, const std::string& to);
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
std::string replaceFirst(const std::string& str, const std::string& from, const std::string& to);
std::string replaceLast(const std::string& str, const std::string& from, const std::string& to);

std::string remove(const std::string& str, const std::string& substr);
std::string removeAll(const std::string& str, const std::string& substr);
std::string removeFirst(const std::string& str, const std::string& substr);
std::string removeLast(const std::string& str, const std::string& substr);

std::vector<std::string> split(const std::string& str, char delimiter);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::vector<std::string> split(const std::string& str, const std::regex& regex);
std::vector<std::string> splitByWhitespace(const std::string& str);
std::vector<std::string> splitLines(const std::string& str);

std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
std::string join(const std::vector<std::string>& strings, char delimiter);
std::string join(const std::vector<std::string>& strings, size_t start, size_t count, const std::string& delimiter);

std::string substring(const std::string& str, size_t start);
std::string substring(const std::string& str, size_t start, size_t length);
std::string left(const std::string& str, size_t count);
std::string right(const std::string& str, size_t count);
std::string mid(const std::string& str, size_t start, size_t count);

size_t count(const std::string& str, const std::string& substr);
size_t count(const std::string& str, char c);
size_t countLines(const std::string& str);
size_t countWords(const std::string& str);

int indexOf(const std::string& str, const std::string& substr, size_t start = 0);
int indexOf(const std::string& str, char c, size_t start = 0);
int lastIndexOf(const std::string& str, const std::string& substr, size_t start = std::string::npos);
int lastIndexOf(const std::string& str, char c, size_t start = std::string::npos);

bool matchRegex(const std::string& str, const std::string& pattern);
bool matchRegex(const std::string& str, const std::regex& regex);
std::vector<std::string> findAllRegex(const std::string& str, const std::string& pattern);
std::vector<std::string> findAllRegex(const std::string& str, const std::regex& regex);
std::string replaceRegex(const std::string& str, const std::string& pattern, const std::string& replacement);
std::string replaceRegex(const std::string& str, const std::regex& regex, const std::string& replacement);

std::string toSnakeCase(const std::string& str);
std::string toCamelCase(const std::string& str);
std::string toPascalCase(const std::string& str);
std::string toKebabCase(const std::string& str);
std::string toTitleCase(const std::string& str);
std::string toSentenceCase(const std::string& str);

std::string escape(const std::string& str);
std::string unescape(const std::string& str);
std::string escapeHtml(const std::string& str);
std::string unescapeHtml(const std::string& str);
std::string escapeXml(const std::string& str);
std::string unescapeXml(const std::string& str);
std::string escapeJson(const std::string& str);
std::string escapeCsv(const std::string& str);

std::string toBase64(const std::string& str);
std::string toBase64(const std::vector<uint8_t>& data);
std::string fromBase64(const std::string& str);
std::vector<uint8_t> fromBase64ToBytes(const std::string& str);

std::string toHex(const std::string& str, bool uppercase = true);
std::string toHex(const std::vector<uint8_t>& data, bool uppercase = true);
std::string fromHex(const std::string& str);
std::vector<uint8_t> fromHexToBytes(const std::string& str);

std::string format(const char* format, ...);
std::string format(const std::string& format, ...);

template<typename T>
std::string toString(const T& value);

template<typename T>
T fromString(const std::string& str);

template<>
std::string toString(const std::string& value);

template<>
std::string fromString(const std::string& str);

template<>
std::string toString(const bool& value);

template<>
bool fromString(const std::string& str);

template<>
std::string toString(const int& value);

template<>
int fromString(const std::string& str);

template<>
std::string toString(const long& value);

template<>
long fromString(const std::string& str);

template<>
std::string toString(const float& value);

template<>
float fromString(const std::string& str);

template<>
std::string toString(const double& value);

template<>
double fromString(const std::string& str);

bool tryParseInt(const std::string& str, int& result);
bool tryParseLong(const std::string& str, long& result);
bool tryParseFloat(const std::string& str, float& result);
bool tryParseDouble(const std::string& str, double& result);
bool tryParseBool(const std::string& str, bool& result);

std::string uuid();
std::string randomString(size_t length);
std::string randomString(size_t length, const std::string& charset);

bool isAlphanumeric(const std::string& str);
bool isAlpha(const std::string& str);
bool isDigit(const std::string& str);
bool isHexDigit(const std::string& str);
bool isNumeric(const std::string& str);
bool isInteger(const std::string& str);
bool isFloat(const std::string& str);
bool isEmail(const std::string& str);
bool isUrl(const std::string& str);
bool isIpAddress(const std::string& str);

std::string levenshteinDistance(const std::string& str1, const std::string& str2);
size_t levenshteinDistance(const std::string& str1, const std::string& str2, size_t maxDistance);
double similarity(const std::string& str1, const std::string& str2);

std::vector<std::string> wrapText(const std::string& text, size_t lineLength);
std::string indent(const std::string& str, size_t count, char c = ' ');
std::string outdent(const std::string& str, size_t count);

class StringBuilder {
public:
    StringBuilder() = default;
    explicit StringBuilder(size_t capacity);
    explicit StringBuilder(const std::string& str);

    StringBuilder& append(const std::string& str);
    StringBuilder& append(const char* str);
    StringBuilder& append(char c);
    StringBuilder& append(int value);
    StringBuilder& append(long value);
    StringBuilder& append(float value);
    StringBuilder& append(double value);
    StringBuilder& append(bool value);

    StringBuilder& appendLine(const std::string& str);
    StringBuilder& appendLine(const char* str);
    StringBuilder& appendLine();

    StringBuilder& appendFormat(const char* format, ...);
    StringBuilder& appendFormat(const std::string& format, ...);

    StringBuilder& insert(size_t index, const std::string& str);
    StringBuilder& insert(size_t index, char c);

    StringBuilder& remove(size_t start, size_t length);
    StringBuilder& removeFirst(size_t count);
    StringBuilder& removeLast(size_t count);
    StringBuilder& clear();

    StringBuilder& replace(const std::string& from, const std::string& to);
    StringBuilder& replaceAll(const std::string& from, const std::string& to);

    size_t length() const;
    size_t capacity() const;
    void reserve(size_t capacity);

    char& operator[](size_t index);
    const char& operator[](size_t index) const;

    std::string toString() const;
    const std::string& str() const;
    operator std::string() const;

private:
    std::string m_buffer;
};

class StringTokenizer {
public:
    StringTokenizer(const std::string& str, const std::string& delimiters = " \t\n\r");
    StringTokenizer(const std::string& str, char delimiter);

    bool hasMoreTokens() const;
    std::string nextToken();
    std::string nextToken(const std::string& delimiters);
    int countTokens() const;
    void reset();

    std::vector<std::string> getAllTokens();

private:
    std::string m_str;
    std::string m_delimiters;
    size_t m_position = 0;
};

}
}

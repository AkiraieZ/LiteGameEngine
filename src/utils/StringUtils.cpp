#include "StringUtils.h"
#include "../core/Logger.h"
#include <random>
#include <cstdarg>
#include <cstdio>

#ifdef _WIN32
#define UUID_SYSTEM_GENERATOR
#endif

namespace LGE {

namespace StringUtils {

bool isEmpty(const std::string& str) {
    return str.empty();
}

bool isBlank(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}

bool startsWith(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) return false;
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

bool contains(const std::string& str, char c) {
    return str.find(c) != std::string::npos;
}

bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) return false;
    return std::equal(str1.begin(), str1.end(), str2.begin(),
        [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
}

bool compare(const std::string& str1, const std::string& str2, bool caseSensitive) {
    if (caseSensitive) {
        return str1 == str2;
    }
    return equalsIgnoreCase(str1, str2);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string capitalize(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    return result;
}

std::string decapitalize(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::tolower(static_cast<unsigned char>(result[0]));
    return result;
}

std::string reverse(const std::string& str) {
    return std::string(str.rbegin(), str.rend());
}

std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string trimStart(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    return str.substr(start);
}

std::string trimEnd(const std::string& str) {
    auto end = str.find_last_not_of(" \t\n\r");
    if (end == std::string::npos) return "";
    return str.substr(0, end + 1);
}

std::string trim(const std::string& str, const std::string& chars) {
    auto start = str.find_first_not_of(chars);
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(chars);
    return str.substr(start, end - start + 1);
}

std::string trimStart(const std::string& str, const std::string& chars) {
    auto start = str.find_first_not_of(chars);
    if (start == std::string::npos) return "";
    return str.substr(start);
}

std::string trimEnd(const std::string& str, const std::string& chars) {
    auto end = str.find_last_not_of(chars);
    if (end == std::string::npos) return "";
    return str.substr(0, end + 1);
}

std::string padLeft(const std::string& str, size_t width, char fillChar) {
    if (str.length() >= width) return str;
    return std::string(width - str.length(), fillChar) + str;
}

std::string padRight(const std::string& str, size_t width, char fillChar) {
    if (str.length() >= width) return str;
    return str + std::string(width - str.length(), fillChar);
}

std::string center(const std::string& str, size_t width, char fillChar) {
    if (str.length() >= width) return str;
    size_t padding = width - str.length();
    size_t left = padding / 2;
    size_t right = padding - left;
    return std::string(left, fillChar) + str + std::string(right, fillChar);
}

std::string repeat(const std::string& str, int count) {
    if (count <= 0) return "";
    std::string result;
    result.reserve(str.length() * count);
    for (int i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

std::string repeat(char c, int count) {
    if (count <= 0) return "";
    return std::string(count, c);
}

std::string replace(const std::string& str, const std::string& from, const std::string& to) {
    return replaceFirst(str, from, to);
}

std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    std::string result;
    size_t start = 0;
    size_t pos;
    while ((pos = str.find(from, start)) != std::string::npos) {
        result += str.substr(start, pos - start);
        result += to;
        start = pos + from.length();
    }
    result += str.substr(start);
    return result;
}

std::string replaceFirst(const std::string& str, const std::string& from, const std::string& to) {
    size_t pos = str.find(from);
    if (pos == std::string::npos) return str;
    return str.substr(0, pos) + to + str.substr(pos + from.length());
}

std::string replaceLast(const std::string& str, const std::string& from, const std::string& to) {
    size_t pos = str.rfind(from);
    if (pos == std::string::npos) return str;
    return str.substr(0, pos) + to + str.substr(pos + from.length());
}

std::string remove(const std::string& str, const std::string& substr) {
    return removeFirst(str, substr);
}

std::string removeAll(const std::string& str, const std::string& substr) {
    return replaceAll(str, substr, "");
}

std::string removeFirst(const std::string& str, const std::string& substr) {
    return replaceFirst(str, substr, "");
}

std::string removeLast(const std::string& str, const std::string& substr) {
    return replaceLast(str, substr, "");
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }
    size_t start = 0;
    size_t pos;
    while ((pos = str.find(delimiter, start)) != std::string::npos) {
        result.push_back(str.substr(start, pos - start));
        start = pos + delimiter.length();
    }
    result.push_back(str.substr(start));
    return result;
}

std::vector<std::string> split(const std::string& str, const std::regex& regex) {
    std::sregex_token_iterator it(str.begin(), str.end(), regex, -1);
    std::sregex_token_iterator end;
    return std::vector<std::string>(it, end);
}

std::vector<std::string> splitByWhitespace(const std::string& str) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

std::vector<std::string> splitLines(const std::string& str) {
    return split(str, '\n');
}

std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

std::string join(const std::vector<std::string>& strings, char delimiter) {
    return join(strings, std::string(1, delimiter));
}

std::string join(const std::vector<std::string>& strings, size_t start, size_t count, const std::string& delimiter) {
    if (start >= strings.size()) return "";
    size_t end = std::min(start + count, strings.size());
    if (end <= start) return "";
    std::string result = strings[start];
    for (size_t i = start + 1; i < end; ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

std::string substring(const std::string& str, size_t start) {
    if (start >= str.length()) return "";
    return str.substr(start);
}

std::string substring(const std::string& str, size_t start, size_t length) {
    if (start >= str.length()) return "";
    return str.substr(start, length);
}

std::string left(const std::string& str, size_t count) {
    if (count >= str.length()) return str;
    return str.substr(0, count);
}

std::string right(const std::string& str, size_t count) {
    if (count >= str.length()) return str;
    return str.substr(str.length() - count);
}

std::string mid(const std::string& str, size_t start, size_t count) {
    return substring(str, start, count);
}

size_t count(const std::string& str, const std::string& substr) {
    if (substr.empty()) return 0;
    size_t count = 0;
    size_t start = 0;
    while ((start = str.find(substr, start)) != std::string::npos) {
        ++count;
        start += substr.length();
    }
    return count;
}

size_t count(const std::string& str, char c) {
    return std::count(str.begin(), str.end(), c);
}

size_t countLines(const std::string& str) {
    if (str.empty()) return 0;
    return count(str, '\n') + 1;
}

size_t countWords(const std::string& str) {
    return splitByWhitespace(str).size();
}

int indexOf(const std::string& str, const std::string& substr, size_t start) {
    size_t pos = str.find(substr, start);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

int indexOf(const std::string& str, char c, size_t start) {
    size_t pos = str.find(c, start);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

int lastIndexOf(const std::string& str, const std::string& substr, size_t start) {
    size_t pos = str.rfind(substr, start);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

int lastIndexOf(const std::string& str, char c, size_t start) {
    size_t pos = str.rfind(c, start);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

bool matchRegex(const std::string& str, const std::string& pattern) {
    try {
        std::regex regex(pattern);
        return std::regex_match(str, regex);
    } catch (const std::regex_error& e) {
        LGE_LOG_ERROR("Regex error: {}", e.what());
        return false;
    }
}

bool matchRegex(const std::string& str, const std::regex& regex) {
    return std::regex_match(str, regex);
}

std::vector<std::string> findAllRegex(const std::string& str, const std::string& pattern) {
    std::vector<std::string> result;
    try {
        std::regex regex(pattern);
        std::sregex_iterator it(str.begin(), str.end(), regex);
        std::sregex_iterator end;
        for (; it != end; ++it) {
            result.push_back(it->str());
        }
    } catch (const std::regex_error& e) {
        LGE_LOG_ERROR("Regex error: {}", e.what());
    }
    return result;
}

std::vector<std::string> findAllRegex(const std::string& str, const std::regex& regex) {
    std::vector<std::string> result;
    std::sregex_iterator it(str.begin(), str.end(), regex);
    std::sregex_iterator end;
    for (; it != end; ++it) {
        result.push_back(it->str());
    }
    return result;
}

std::string replaceRegex(const std::string& str, const std::string& pattern, const std::string& replacement) {
    try {
        std::regex regex(pattern);
        return std::regex_replace(str, regex, replacement);
    } catch (const std::regex_error& e) {
        LGE_LOG_ERROR("Regex error: {}", e.what());
        return str;
    }
}

std::string replaceRegex(const std::string& str, const std::regex& regex, const std::string& replacement) {
    return std::regex_replace(str, regex, replacement);
}

std::string toSnakeCase(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (std::isupper(static_cast<unsigned char>(c))) {
            if (i > 0) result += '_';
            result += std::tolower(static_cast<unsigned char>(c));
        } else if (c == ' ' || c == '-') {
            result += '_';
        } else {
            result += c;
        }
    }
    return result;
}

std::string toCamelCase(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    bool nextUpper = false;
    for (char c : str) {
        if (c == '_' || c == ' ' || c == '-') {
            nextUpper = true;
        } else if (nextUpper) {
            result += std::toupper(static_cast<unsigned char>(c));
            nextUpper = false;
        } else {
            result += std::tolower(static_cast<unsigned char>(c));
        }
    }
    return result;
}

std::string toPascalCase(const std::string& str) {
    std::string result = toCamelCase(str);
    if (!result.empty()) {
        result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    }
    return result;
}

std::string toKebabCase(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (std::isupper(static_cast<unsigned char>(c))) {
            if (i > 0) result += '-';
            result += std::tolower(static_cast<unsigned char>(c));
        } else if (c == ' ' || c == '_') {
            result += '-';
        } else {
            result += c;
        }
    }
    return result;
}

std::string toTitleCase(const std::string& str) {
    std::string result = str;
    bool newWord = true;
    for (size_t i = 0; i < result.length(); ++i) {
        if (std::isspace(static_cast<unsigned char>(result[i]))) {
            newWord = true;
        } else if (newWord) {
            result[i] = std::toupper(static_cast<unsigned char>(result[i]));
            newWord = false;
        } else {
            result[i] = std::tolower(static_cast<unsigned char>(result[i]));
        }
    }
    return result;
}

std::string toSentenceCase(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    for (size_t i = 1; i < result.length(); ++i) {
        result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    }
    return result;
}

std::string escape(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string unescape(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += str[i + 1]; break;
            }
            ++i;
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string escapeHtml(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    for (char c : str) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '\"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string unescapeHtml(const std::string& str) {
    std::string result = replaceAll(str, "&amp;", "&");
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&#39;", "'");
    return result;
}

std::string escapeXml(const std::string& str) {
    return escapeHtml(str);
}

std::string unescapeXml(const std::string& str) {
    return unescapeHtml(str);
}

std::string escapeJson(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:
                if (c >= 0 && c <= 0x1F) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string escapeCsv(const std::string& str) {
    bool needsQuotes = contains(str, ',') || contains(str, '\"') || contains(str, '\n');
    if (!needsQuotes) return str;
    std::string result = "\"";
    result += replaceAll(str, "\"", "\"\"");
    result += "\"";
    return result;
}

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string toBase64(const std::string& str) {
    return toBase64(std::vector<uint8_t>(str.begin(), str.end()));
}

std::string toBase64(const std::vector<uint8_t>& data) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    size_t in_len = data.size();
    const unsigned char* bytes_to_encode = data.data();

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];
        while (i++ < 3)
            ret += '=';
    }
    return ret;
}

std::string fromBase64(const std::string& encoded_string) {
    auto bytes = fromBase64ToBytes(encoded_string);
    return std::string(bytes.begin(), bytes.end());
}

std::vector<uint8_t> fromBase64ToBytes(const std::string& encoded_string) {
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<uint8_t> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        for (j = 0; j < 4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        for (j = 0; j < i - 1; j++)
            ret.push_back(char_array_3[j]);
    }
    return ret;
}

std::string toHex(const std::string& str, bool uppercase) {
    return toHex(std::vector<uint8_t>(str.begin(), str.end()), uppercase);
}

std::string toHex(const std::vector<uint8_t>& data, bool uppercase) {
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    std::string result;
    result.reserve(data.size() * 2);
    for (uint8_t byte : data) {
        result += digits[(byte >> 4) & 0xF];
        result += digits[byte & 0xF];
    }
    return result;
}

std::string fromHex(const std::string& str) {
    auto bytes = fromHexToBytes(str);
    return std::string(bytes.begin(), bytes.end());
}

std::vector<uint8_t> fromHexToBytes(const std::string& str) {
    std::vector<uint8_t> result;
    if (str.length() % 2 != 0) return result;
    result.reserve(str.length() / 2);
    for (size_t i = 0; i < str.length(); i += 2) {
        std::string byteStr = str.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
        result.push_back(byte);
    }
    return result;
}

std::string format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int size = vsnprintf(nullptr, 0, format, args);
    va_end(args);
    if (size <= 0) return "";
    std::string result(size + 1, '\0');
    va_list args2;
    va_start(args2, format);
    vsnprintf(&result[0], result.size(), format, args2);
    va_end(args2);
    result.resize(size);
    return result;
}

std::string format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    int size = vsnprintf(nullptr, 0, format.c_str(), args);
    va_end(args);
    if (size <= 0) return "";
    std::string result(size + 1, '\0');
    va_list args2;
    va_start(args2, format);
    vsnprintf(&result[0], result.size(), format.c_str(), args2);
    va_end(args2);
    result.resize(size);
    return result;
}

template<>
std::string toString(const std::string& value) {
    return value;
}

template<>
std::string fromString(const std::string& str) {
    return str;
}

template<>
std::string toString(const bool& value) {
    return value ? "true" : "false";
}

template<>
bool fromString(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "y";
}

template<>
std::string toString(const int& value) {
    return std::to_string(value);
}

template<>
int fromString(const std::string& str) {
    return std::stoi(str);
}

template<>
std::string toString(const long& value) {
    return std::to_string(value);
}

template<>
long fromString(const std::string& str) {
    return std::stol(str);
}

template<>
std::string toString(const float& value) {
    return std::to_string(value);
}

template<>
float fromString(const std::string& str) {
    return std::stof(str);
}

template<>
std::string toString(const double& value) {
    return std::to_string(value);
}

template<>
double fromString(const std::string& str) {
    return std::stod(str);
}

bool tryParseInt(const std::string& str, int& result) {
    try {
        result = std::stoi(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool tryParseLong(const std::string& str, long& result) {
    try {
        result = std::stol(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool tryParseFloat(const std::string& str, float& result) {
    try {
        result = std::stof(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool tryParseDouble(const std::string& str, double& result) {
    try {
        result = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool tryParseBool(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "y") {
        result = true;
        return true;
    }
    if (lower == "false" || lower == "0" || lower == "no" || lower == "n") {
        result = false;
        return true;
    }
    return false;
}

std::string uuid() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    uint32_t data[4];
    data[0] = dis(gen);
    data[1] = dis(gen);
    data[2] = dis(gen);
    data[3] = dis(gen);

    data[1] = (data[1] & 0xFFFF0FFF) | 0x00004000;
    data[2] = (data[2] & 0x3FFFFFFF) | 0x80000000;

    return format("%08x-%04x-%04x-%04x-%012x",
        data[0],
        (data[1] >> 16) & 0xFFFF,
        data[1] & 0xFFFF,
        (data[2] >> 16) & 0xFFFF,
        ((uint64_t)(data[2] & 0xFFFF) << 32) | data[3]);
}

std::string randomString(size_t length) {
    const std::string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return randomString(length, charset);
}

std::string randomString(size_t length, const std::string& charset) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, charset.length() - 1);

    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    return result;
}

bool isAlphanumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c));
    });
}

bool isAlpha(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    });
}

bool isDigit(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    });
}

bool isHexDigit(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
    });
}

bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    bool hasDot = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    return true;
}

bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    if (start >= str.length()) return false;
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    return true;
}

bool isFloat(const std::string& str) {
    return isNumeric(str);
}

bool isEmail(const std::string& str) {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return matchRegex(str, pattern);
}

bool isUrl(const std::string& str) {
    const std::regex pattern(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)");
    return matchRegex(str, pattern);
}

bool isIpAddress(const std::string& str) {
    const std::regex ipv4Pattern(R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
    const std::regex ipv6Pattern(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$)");
    return matchRegex(str, ipv4Pattern) || matchRegex(str, ipv6Pattern);
}

size_t levenshteinDistance(const std::string& str1, const std::string& str2, size_t maxDistance) {
    size_t m = str1.length();
    size_t n = str2.length();

    if (m > n) {
        return levenshteinDistance(str2, str1, maxDistance);
    }

    if (n - m > maxDistance) {
        return maxDistance + 1;
    }

    std::vector<size_t> current(m + 1);
    std::vector<size_t> previous(m + 1);

    for (size_t j = 0; j <= m; ++j) {
        previous[j] = j;
    }

    for (size_t i = 1; i <= n; ++i) {
        current[0] = i;
        size_t minCurrent = i;

        for (size_t j = 1; j <= m; ++j) {
            size_t cost = (str1[j - 1] == str2[i - 1]) ? 0 : 1;
            current[j] = std::min({ previous[j] + 1, current[j - 1] + 1, previous[j - 1] + cost });
            minCurrent = std::min(minCurrent, current[j]);
        }

        if (minCurrent > maxDistance) {
            return maxDistance + 1;
        }

        std::swap(current, previous);
    }

    return previous[m];
}

double similarity(const std::string& str1, const std::string& str2) {
    size_t maxLen = std::max(str1.length(), str2.length());
    if (maxLen == 0) return 1.0;
    size_t dist = levenshteinDistance(str1, str2, maxLen);
    return 1.0 - (static_cast<double>(dist) / maxLen);
}

std::vector<std::string> wrapText(const std::string& text, size_t lineLength) {
    std::vector<std::string> result;
    std::istringstream words(text);
    std::string currentLine;
    std::string word;

    while (words >> word) {
        if (currentLine.length() + word.length() + 1 > lineLength) {
            if (!currentLine.empty()) {
                result.push_back(currentLine);
            }
            currentLine = word;
        } else {
            if (!currentLine.empty()) {
                currentLine += " ";
            }
            currentLine += word;
        }
    }

    if (!currentLine.empty()) {
        result.push_back(currentLine);
    }

    return result;
}

std::string indent(const std::string& str, size_t count, char c) {
    std::string indentStr(count, c);
    std::vector<std::string> lines = splitLines(str);
    std::string result;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) result += "\n";
        result += indentStr + lines[i];
    }
    return result;
}

std::string outdent(const std::string& str, size_t count) {
    std::vector<std::string> lines = splitLines(str);
    std::string result;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) result += "\n";
        size_t start = 0;
        while (start < lines[i].length() && start < count && std::isspace(static_cast<unsigned char>(lines[i][start]))) {
            ++start;
        }
        result += lines[i].substr(start);
    }
    return result;
}

StringBuilder::StringBuilder(size_t capacity) {
    m_buffer.reserve(capacity);
}

StringBuilder::StringBuilder(const std::string& str) : m_buffer(str) {}

StringBuilder& StringBuilder::append(const std::string& str) {
    m_buffer += str;
    return *this;
}

StringBuilder& StringBuilder::append(const char* str) {
    m_buffer += str;
    return *this;
}

StringBuilder& StringBuilder::append(char c) {
    m_buffer += c;
    return *this;
}

StringBuilder& StringBuilder::append(int value) {
    m_buffer += std::to_string(value);
    return *this;
}

StringBuilder& StringBuilder::append(long value) {
    m_buffer += std::to_string(value);
    return *this;
}

StringBuilder& StringBuilder::append(float value) {
    m_buffer += std::to_string(value);
    return *this;
}

StringBuilder& StringBuilder::append(double value) {
    m_buffer += std::to_string(value);
    return *this;
}

StringBuilder& StringBuilder::append(bool value) {
    m_buffer += value ? "true" : "false";
    return *this;
}

StringBuilder& StringBuilder::appendLine(const std::string& str) {
    m_buffer += str;
    m_buffer += '\n';
    return *this;
}

StringBuilder& StringBuilder::appendLine(const char* str) {
    m_buffer += str;
    m_buffer += '\n';
    return *this;
}

StringBuilder& StringBuilder::appendLine() {
    m_buffer += '\n';
    return *this;
}

StringBuilder& StringBuilder::insert(size_t index, const std::string& str) {
    m_buffer.insert(index, str);
    return *this;
}

StringBuilder& StringBuilder::insert(size_t index, char c) {
    m_buffer.insert(index, 1, c);
    return *this;
}

StringBuilder& StringBuilder::remove(size_t start, size_t length) {
    m_buffer.erase(start, length);
    return *this;
}

StringBuilder& StringBuilder::removeFirst(size_t count) {
    if (count > m_buffer.length()) count = m_buffer.length();
    m_buffer.erase(0, count);
    return *this;
}

StringBuilder& StringBuilder::removeLast(size_t count) {
    if (count > m_buffer.length()) count = m_buffer.length();
    m_buffer.erase(m_buffer.length() - count, count);
    return *this;
}

StringBuilder& StringBuilder::clear() {
    m_buffer.clear();
    return *this;
}

StringBuilder& StringBuilder::replace(const std::string& from, const std::string& to) {
    m_buffer = replaceFirst(m_buffer, from, to);
    return *this;
}

StringBuilder& StringBuilder::replaceAll(const std::string& from, const std::string& to) {
    m_buffer = replaceAll(m_buffer, from, to);
    return *this;
}

size_t StringBuilder::length() const {
    return m_buffer.length();
}

size_t StringBuilder::capacity() const {
    return m_buffer.capacity();
}

void StringBuilder::reserve(size_t capacity) {
    m_buffer.reserve(capacity);
}

char& StringBuilder::operator[](size_t index) {
    return m_buffer[index];
}

const char& StringBuilder::operator[](size_t index) const {
    return m_buffer[index];
}

std::string StringBuilder::toString() const {
    return m_buffer;
}

const std::string& StringBuilder::str() const {
    return m_buffer;
}

StringBuilder::operator std::string() const {
    return m_buffer;
}

StringTokenizer::StringTokenizer(const std::string& str, const std::string& delimiters)
    : m_str(str), m_delimiters(delimiters) {}

StringTokenizer::StringTokenizer(const std::string& str, char delimiter)
    : m_str(str), m_delimiters(1, delimiter) {}

bool StringTokenizer::hasMoreTokens() const {
    size_t pos = m_str.find_first_not_of(m_delimiters, m_position);
    return pos != std::string::npos;
}

std::string StringTokenizer::nextToken() {
    size_t start = m_str.find_first_not_of(m_delimiters, m_position);
    if (start == std::string::npos) {
        m_position = m_str.length();
        return "";
    }
    size_t end = m_str.find_first_of(m_delimiters, start);
    if (end == std::string::npos) {
        end = m_str.length();
    }
    m_position = end;
    return m_str.substr(start, end - start);
}

std::string StringTokenizer::nextToken(const std::string& delimiters) {
    m_delimiters = delimiters;
    return nextToken();
}

int StringTokenizer::countTokens() const {
    int count = 0;
    size_t pos = 0;
    while (true) {
        size_t start = m_str.find_first_not_of(m_delimiters, pos);
        if (start == std::string::npos) break;
        ++count;
        size_t end = m_str.find_first_of(m_delimiters, start);
        if (end == std::string::npos) break;
        pos = end;
    }
    return count;
}

void StringTokenizer::reset() {
    m_position = 0;
}

std::vector<std::string> StringTokenizer::getAllTokens() {
    reset();
    std::vector<std::string> tokens;
    while (hasMoreTokens()) {
        tokens.push_back(nextToken());
    }
    return tokens;
}
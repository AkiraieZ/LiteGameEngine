#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

namespace LGE {
namespace Utils {

class Path {
public:
    Path() : path_() {}
    Path(const std::string& path) : path_(path) { normalize(); }
    Path(const char* path) : path_(path ? path : "") { normalize(); }

    static Path current() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        return Path(buffer);
#else
        char buffer[PATH_MAX];
        getcwd(buffer, PATH_MAX);
        return Path(buffer);
#endif
    }

    static Path home() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, buffer))) {
            return Path(buffer);
        }
        return Path("");
#else
        const char* home = getenv("HOME");
        return Path(home ? home : "");
#endif
    }

    static Path temp() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetTempPathA(MAX_PATH, buffer);
        return Path(buffer);
#else
        const char* tmp = getenv("TMPDIR");
        if (!tmp) tmp = getenv("TMP");
        if (!tmp) tmp = getenv("TEMP");
        if (!tmp) tmp = "/tmp";
        return Path(tmp);
#endif
    }

    std::string toString() const { return path_; }
    const char* c_str() const { return path_.c_str(); }

    bool isEmpty() const { return path_.empty(); }

    bool isAbsolute() const {
        if (path_.empty()) return false;
#ifdef _WIN32
        return (path_.size() >= 2 && std::isalpha(path_[0]) && path_[1] == ':') ||
               (path_.size() >= 2 && path_[0] == '\\' && path_[1] == '\\');
#else
        return path_[0] == '/';
#endif
    }

    bool isRelative() const { return !isAbsolute(); }

    Path parent() const {
        if (path_.empty()) return Path();
        size_t pos = findLastSeparator();
        if (pos == std::string::npos) return Path("");
        return Path(path_.substr(0, pos));
    }

    std::string filename() const {
        size_t pos = findLastSeparator();
        if (pos == std::string::npos) return path_;
        return path_.substr(pos + 1);
    }

    std::string stem() const {
        std::string name = filename();
        size_t pos = name.find_last_of('.');
        if (pos == std::string::npos) return name;
        return name.substr(0, pos);
    }

    std::string extension() const {
        std::string name = filename();
        size_t pos = name.find_last_of('.');
        if (pos == std::string::npos || pos == 0) return "";
        return name.substr(pos);
    }

    std::string extensionWithoutDot() const {
        std::string ext = extension();
        if (!ext.empty() && ext[0] == '.') return ext.substr(1);
        return ext;
    }

    bool hasExtension(const std::string& ext) const {
        std::string thisExt = extension();
        std::string checkExt = ext;
        if (!checkExt.empty() && checkExt[0] != '.') {
            checkExt = "." + checkExt;
        }
        std::transform(thisExt.begin(), thisExt.end(), thisExt.begin(), ::tolower);
        std::transform(checkExt.begin(), checkExt.end(), checkExt.begin(), ::tolower);
        return thisExt == checkExt;
    }

    Path join(const Path& other) const {
        if (other.isEmpty()) return *this;
        if (other.isAbsolute()) return other;
        if (path_.empty()) return other;
        
        std::string result = path_;
        char last = result.back();
        if (!isSeparator(last)) {
            result += separator();
        }
        result += other.path_;
        return Path(result);
    }

    Path operator/(const Path& other) const {
        return join(other);
    }

    Path& operator/=(const Path& other) {
        *this = join(other);
        return *this;
    }

    Path normalize() const {
        Path p = *this;
        p.normalize();
        return p;
    }

    Path absolute() const {
        if (isAbsolute()) return normalize();
        return current().join(*this).normalize();
    }

    Path relative(const Path& base) const {
        Path absThis = this->absolute();
        Path absBase = base.absolute();

        std::vector<std::string> thisParts = absThis.split();
        std::vector<std::string> baseParts = absBase.split();

        size_t common = 0;
        while (common < thisParts.size() && common < baseParts.size() && 
               thisParts[common] == baseParts[common]) {
            ++common;
        }

        std::string result;
        for (size_t i = common; i < baseParts.size(); ++i) {
            if (!result.empty()) result += separator();
            result += "..";
        }

        for (size_t i = common; i < thisParts.size(); ++i) {
            if (!result.empty()) result += separator();
            result += thisParts[i];
        }

        return Path(result.empty() ? "." : result);
    }

    bool exists() const {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(path_.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES;
#else
        struct stat st;
        return stat(path_.c_str(), &st) == 0;
#endif
    }

    bool isFile() const {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(path_.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat st;
        return stat(path_.c_str(), &st) == 0 && S_ISREG(st.st_mode);
#endif
    }

    bool isDirectory() const {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(path_.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat st;
        return stat(path_.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
#endif
    }

    bool isSymlink() const {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(path_.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_REPARSE_POINT);
#else
        struct stat st;
        return lstat(path_.c_str(), &st) == 0 && S_ISLNK(st.st_mode);
#endif
    }

    std::vector<std::string> split() const {
        std::vector<std::string> parts;
        std::string current;

        for (char c : path_) {
            if (isSeparator(c)) {
                if (!current.empty()) {
                    parts.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            parts.push_back(current);
        }

        return parts;
    }

    bool operator==(const Path& other) const {
        return path_ == other.path_;
    }

    bool operator!=(const Path& other) const {
        return path_ != other.path_;
    }

    bool operator<(const Path& other) const {
        return path_ < other.path_;
    }

    static char separator() {
#ifdef _WIN32
        return '\\';
#else
        return '/';
#endif
    }

    static std::string separators() {
#ifdef _WIN32
        return "\\/";
#else
        return "/";
#endif
    }

    static bool isSeparator(char c) {
        return separators().find(c) != std::string::npos;
    }

private:
    void normalize() {
        if (path_.empty()) return;

        std::string result;
        result.reserve(path_.size());

#ifdef _WIN32
        for (char c : path_) {
            if (c == '/') {
                result += '\\';
            } else {
                result += c;
            }
        }
#else
        result = path_;
#endif

        std::vector<std::string> parts;
        std::string current;

        for (size_t i = 0; i < result.size(); ++i) {
            char c = result[i];
            if (isSeparator(c)) {
                if (!current.empty()) {
                    parts.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            parts.push_back(current);
        }

        std::vector<std::string> normalizedParts;
        for (const auto& part : parts) {
            if (part == ".") {
                continue;
            } else if (part == "..") {
                if (!normalizedParts.empty() && normalizedParts.back() != "..") {
                    normalizedParts.pop_back();
                } else {
                    normalizedParts.push_back(part);
                }
            } else {
                normalizedParts.push_back(part);
            }
        }

        result.clear();
#ifdef _WIN32
        if (path_.size() >= 2 && std::isalpha(path_[0]) && path_[1] == ':') {
            result += path_.substr(0, 2);
            result += separator();
        }
#else
        if (path_[0] == '/') {
            result += separator();
        }
#endif

        for (size_t i = 0; i < normalizedParts.size(); ++i) {
            if (i > 0 || !result.empty()) {
                result += separator();
            }
            result += normalizedParts[i];
        }

        path_ = result;
    }

    size_t findLastSeparator() const {
        for (size_t i = path_.size(); i > 0; --i) {
            if (isSeparator(path_[i - 1])) {
                return i - 1;
            }
        }
        return std::string::npos;
    }

    std::string path_;
};

class Directory {
public:
    static bool create(const Path& path) {
        if (path.exists()) {
            return path.isDirectory();
        }
        
        Path parent = path.parent();
        if (!parent.isEmpty() && !parent.exists()) {
            if (!create(parent)) {
                return false;
            }
        }

#ifdef _WIN32
        return CreateDirectoryA(path.toString().c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
        return mkdir(path.toString().c_str(), 0755) == 0 || errno == EEXIST;
#endif
    }

    static bool remove(const Path& path, bool recursive = false) {
        if (!path.exists()) return false;
        
        if (!path.isDirectory()) return false;

        if (recursive) {
            std::vector<Path> entries = list(path);
            for (const auto& entry : entries) {
                if (entry.isDirectory()) {
                    if (!remove(entry, true)) return false;
                } else {
                    if (!File::remove(entry)) return false;
                }
            }
        }

#ifdef _WIN32
        return RemoveDirectoryA(path.toString().c_str()) != 0;
#else
        return rmdir(path.toString().c_str()) == 0;
#endif
    }

    static std::vector<Path> list(const Path& path) {
        std::vector<Path> result;
        
        if (!path.isDirectory()) return result;

#ifdef _WIN32
        WIN32_FIND_DATAA findData;
        std::string searchPath = path.toString() + "\\*";
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                    result.push_back(path.join(Path(findData.cFileName)));
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
#else
        DIR* dir = opendir(path.toString().c_str());
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    result.push_back(path.join(Path(entry->d_name)));
                }
            }
            closedir(dir);
        }
#endif

        return result;
    }

    static std::vector<Path> listFiles(const Path& path, const std::string& pattern = "*") {
        std::vector<Path> all = list(path);
        std::vector<Path> result;
        
        for (const auto& entry : all) {
            if (entry.isFile()) {
                if (matchPattern(entry.filename(), pattern)) {
                    result.push_back(entry);
                }
            }
        }
        
        return result;
    }

    static std::vector<Path> listDirectories(const Path& path) {
        std::vector<Path> all = list(path);
        std::vector<Path> result;
        
        for (const auto& entry : all) {
            if (entry.isDirectory()) {
                result.push_back(entry);
            }
        }
        
        return result;
    }

    static bool copy(const Path& src, const Path& dst, bool recursive = false) {
        if (!src.exists() || !src.isDirectory()) return false;
        
        if (!dst.exists()) {
            if (!create(dst)) return false;
        }
        
        std::vector<Path> entries = list(src);
        for (const auto& entry : entries) {
            Path dstEntry = dst.join(Path(entry.filename()));
            if (entry.isDirectory()) {
                if (recursive) {
                    if (!copy(entry, dstEntry, true)) return false;
                }
            } else {
                if (!File::copy(entry, dstEntry)) return false;
            }
        }
        
        return true;
    }

    static bool move(const Path& src, const Path& dst) {
        if (!src.exists()) return false;
        
#ifdef _WIN32
        return MoveFileA(src.toString().c_str(), dst.toString().c_str()) != 0;
#else
        return rename(src.toString().c_str(), dst.toString().c_str()) == 0;
#endif
    }

    static bool matchPattern(const std::string& filename, const std::string& pattern) {
        if (pattern == "*") return true;
        
        size_t i = 0, j = 0;
        size_t starIdx = std::string::npos;
        size_t matchIdx = 0;
        
        while (i < filename.size()) {
            if (j < pattern.size() && (pattern[j] == filename[i] || pattern[j] == '?')) {
                ++i;
                ++j;
            } else if (j < pattern.size() && pattern[j] == '*') {
                starIdx = j++;
                matchIdx = i;
            } else if (starIdx != std::string::npos) {
                j = starIdx + 1;
                i = ++matchIdx;
            } else {
                return false;
            }
        }
        
        while (j < pattern.size() && pattern[j] == '*') {
            ++j;
        }
        
        return j == pattern.size();
    }
};

class File {
public:
    static bool remove(const Path& path) {
        if (!path.exists() || path.isDirectory()) return false;
#ifdef _WIN32
        return DeleteFileA(path.toString().c_str()) != 0;
#else
        return unlink(path.toString().c_str()) == 0;
#endif
    }

    static bool exists(const Path& path) {
        return path.exists() && path.isFile();
    }

    static size_t size(const Path& path) {
        if (!path.isFile()) return 0;
#ifdef _WIN32
        WIN32_FILE_ATTRIBUTE_DATA fileData;
        if (!GetFileAttributesExA(path.toString().c_str(), GetFileExInfoStandard, &fileData)) {
            return 0;
        }
        LARGE_INTEGER size;
        size.HighPart = fileData.nFileSizeHigh;
        size.LowPart = fileData.nFileSizeLow;
        return static_cast<size_t>(size.QuadPart);
#else
        struct stat st;
        if (stat(path.toString().c_str(), &st) != 0) return 0;
        return static_cast<size_t>(st.st_size);
#endif
    }

    static bool copy(const Path& src, const Path& dst) {
        if (!src.isFile()) return false;
        
#ifdef _WIN32
        return CopyFileA(src.toString().c_str(), dst.toString().c_str(), FALSE) != 0;
#else
        FILE* in = fopen(src.toString().c_str(), "rb");
        if (!in) return false;
        
        FILE* out = fopen(dst.toString().c_str(), "wb");
        if (!out) {
            fclose(in);
            return false;
        }
        
        char buffer[8192];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), in)) > 0) {
            if (fwrite(buffer, 1, bytesRead, out) != bytesRead) {
                fclose(in);
                fclose(out);
                return false;
            }
        }
        
        fclose(in);
        fclose(out);
        return true;
#endif
    }

    static bool move(const Path& src, const Path& dst) {
        if (!src.isFile()) return false;
        
#ifdef _WIN32
        return MoveFileA(src.toString().c_str(), dst.toString().c_str()) != 0;
#else
        return rename(src.toString().c_str(), dst.toString().c_str()) == 0;
#endif
    }

    static bool setReadOnly(const Path& path, bool readOnly) {
        if (!path.isFile()) return false;
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(path.toString().c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) return false;
        if (readOnly) {
            attrs |= FILE_ATTRIBUTE_READONLY;
        } else {
            attrs &= ~FILE_ATTRIBUTE_READONLY;
        }
        return SetFileAttributesA(path.toString().c_str(), attrs) != 0;
#else
        struct stat st;
        if (stat(path.toString().c_str(), &st) != 0) return false;
        if (readOnly) {
            st.st_mode &= ~S_IWUSR;
            st.st_mode &= ~S_IWGRP;
            st.st_mode &= ~S_IWOTH;
        } else {
            st.st_mode |= S_IWUSR;
        }
        return chmod(path.toString().c_str(), st.st_mode) == 0;
#endif
    }
};

}
}

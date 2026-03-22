#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdint>
#include <functional>

namespace LGE {

namespace FileUtils {

bool fileExists(const std::string& path);
bool directoryExists(const std::string& path);
bool createDirectory(const std::string& path);
bool createDirectories(const std::string& path);
bool deleteFile(const std::string& path);
bool deleteDirectory(const std::string& path, bool recursive = false);
bool copyFile(const std::string& src, const std::string& dst, bool overwrite = true);
bool copyDirectory(const std::string& src, const std::string& dst, bool overwrite = true);
bool moveFile(const std::string& src, const std::string& dst);
bool renameFile(const std::string& oldPath, const std::string& newPath);

std::string getFileExtension(const std::string& path);
std::string getFileName(const std::string& path, bool withExtension = true);
std::string getDirectoryName(const std::string& path);
std::string getAbsolutePath(const std::string& path);
std::string getRelativePath(const std::string& path, const std::string& basePath);
std::string joinPaths(const std::string& path1, const std::string& path2);
std::string normalizePath(const std::string& path);

uint64_t getFileSize(const std::string& path);
std::time_t getFileLastModifiedTime(const std::string& path);
bool setFileLastModifiedTime(const std::string& path, std::time_t time);

bool readFileToString(const std::string& path, std::string& content);
bool readFileToString(const std::string& path, std::string& content, const std::string& encoding);
std::string readFileToString(const std::string& path);
bool readFileToBytes(const std::string& path, std::vector<uint8_t>& data);
std::vector<uint8_t> readFileToBytes(const std::string& path);
bool readLines(const std::string& path, std::vector<std::string>& lines);
std::vector<std::string> readLines(const std::string& path);

bool writeStringToFile(const std::string& path, const std::string& content, bool append = false);
bool writeBytesToFile(const std::string& path, const std::vector<uint8_t>& data, bool append = false);
bool writeLinesToFile(const std::string& path, const std::vector<std::string>& lines, bool append = false);

void iterateDirectory(const std::string& path, std::function<void(const std::string&)> callback, bool recursive = true);
std::vector<std::string> listFiles(const std::string& path, bool recursive = true, bool includeDirectories = false);
std::vector<std::string> listFilesWithExtension(const std::string& path, const std::string& extension, bool recursive = true);

bool isAbsolutePath(const std::string& path);
bool isRelativePath(const std::string& path);
bool isFile(const std::string& path);
bool isDirectory(const std::string& path);
bool isSymLink(const std::string& path);
bool isReadOnly(const std::string& path);
bool setReadOnly(const std::string& path, bool readOnly);

std::string getCurrentWorkingDirectory();
bool setCurrentWorkingDirectory(const std::string& path);
std::string getHomeDirectory();
std::string getTempDirectory();
std::string getTempFileName(const std::string& prefix = "", const std::string& extension = "");

class FileWatcher {
public:
    FileWatcher() = default;
    ~FileWatcher();

    void watch(const std::string& path, bool recursive = true);
    void unwatch(const std::string& path);
    void clear();

    void update();

    enum class EventType {
        Created,
        Modified,
        Deleted,
        Renamed
    };

    struct FileEvent {
        EventType type;
        std::string path;
        std::string oldPath;
    };

    void setCallback(std::function<void(const FileEvent&)> callback);

private:
    struct WatchEntry {
        std::string path;
        bool recursive;
        std::filesystem::file_time_type lastWriteTime;
    };

    std::vector<WatchEntry> m_watches;
    std::function<void(const FileEvent&)> m_callback;
};

class MemoryMappedFile {
public:
    MemoryMappedFile();
    ~MemoryMappedFile();

    bool open(const std::string& path, bool readOnly = true);
    void close();
    bool isOpen() const { return m_data != nullptr; }

    void* data() { return m_data; }
    const void* data() const { return m_data; }
    size_t size() const { return m_size; }

    void* getData(size_t offset = 0) { return static_cast<uint8_t*>(m_data) + offset; }
    const void* getData(size_t offset = 0) const { return static_cast<const uint8_t*>(m_data) + offset; }

private:
    void* m_data = nullptr;
    size_t m_size = 0;
    void* m_fileHandle = nullptr;
    void* m_mappingHandle = nullptr;
};

}
}

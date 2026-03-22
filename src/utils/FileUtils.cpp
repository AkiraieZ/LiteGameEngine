#include "FileUtils.h"
#include "../core/Logger.h"
#include <algorithm>
#include <cstring>
#include <random>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace LGE {

namespace FileUtils {

bool fileExists(const std::string& path) {
    std::filesystem::path p(path);
    return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
}

bool directoryExists(const std::string& path) {
    std::filesystem::path p(path);
    return std::filesystem::exists(p) && std::filesystem::is_directory(p);
}

bool createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directory(path);
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to create directory {}: {}", path, e.what());
        return false;
    }
}

bool createDirectories(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to create directories {}: {}", path, e.what());
        return false;
    }
}

bool deleteFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to delete file {}: {}", path, e.what());
        return false;
    }
}

bool deleteDirectory(const std::string& path, bool recursive) {
    try {
        if (recursive) {
            return std::filesystem::remove_all(path) > 0;
        } else {
            return std::filesystem::remove(path);
        }
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to delete directory {}: {}", path, e.what());
        return false;
    }
}

bool copyFile(const std::string& src, const std::string& dst, bool overwrite) {
    try {
        auto options = overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none;
        return std::filesystem::copy_file(src, dst, options);
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to copy file from {} to {}: {}", src, dst, e.what());
        return false;
    }
}

bool copyDirectory(const std::string& src, const std::string& dst, bool overwrite) {
    try {
        auto options = overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none;
        options |= std::filesystem::copy_options::recursive;
        std::filesystem::copy(src, dst, options);
        return true;
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to copy directory from {} to {}: {}", src, dst, e.what());
        return false;
    }
}

bool moveFile(const std::string& src, const std::string& dst) {
    try {
        std::filesystem::rename(src, dst);
        return true;
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to move file from {} to {}: {}", src, dst, e.what());
        return false;
    }
}

bool renameFile(const std::string& oldPath, const std::string& newPath) {
    return moveFile(oldPath, newPath);
}

std::string getFileExtension(const std::string& path) {
    std::filesystem::path p(path);
    return p.extension().string();
}

std::string getFileName(const std::string& path, bool withExtension) {
    std::filesystem::path p(path);
    if (withExtension) {
        return p.filename().string();
    } else {
        return p.stem().string();
    }
}

std::string getDirectoryName(const std::string& path) {
    std::filesystem::path p(path);
    return p.parent_path().string();
}

std::string getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get absolute path for {}: {}", path, e.what());
        return path;
    }
}

std::string getRelativePath(const std::string& path, const std::string& basePath) {
    try {
        return std::filesystem::relative(path, basePath).string();
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get relative path for {} with base {}: {}", path, basePath, e.what());
        return path;
    }
}

std::string joinPaths(const std::string& path1, const std::string& path2) {
    std::filesystem::path p(path1);
    p /= path2;
    return p.string();
}

std::string normalizePath(const std::string& path) {
    try {
        return std::filesystem::path(path).lexically_normal().string();
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to normalize path {}: {}", path, e.what());
        return path;
    }
}

uint64_t getFileSize(const std::string& path) {
    try {
        return static_cast<uint64_t>(std::filesystem::file_size(path));
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get file size for {}: {}", path, e.what());
        return 0;
    }
}

std::time_t getFileLastModifiedTime(const std::string& path) {
    try {
        auto ftime = std::filesystem::last_write_time(path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get file modified time for {}: {}", path, e.what());
        return 0;
    }
}

bool setFileLastModifiedTime(const std::string& path, std::time_t time) {
    try {
        auto sctp = std::chrono::system_clock::from_time_t(time);
        auto ftime = std::chrono::time_point_cast<std::filesystem::file_time_type::duration>(
            sctp - std::chrono::system_clock::now() + std::filesystem::file_time_type::clock::now());
        std::filesystem::last_write_time(path, ftime);
        return true;
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to set file modified time for {}: {}", path, e.what());
        return false;
    }
}

bool readFileToString(const std::string& path, std::string& content) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for reading: {}", path);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}

bool readFileToString(const std::string& path, std::string& content, const std::string& encoding) {
    return readFileToString(path, content);
}

std::string readFileToString(const std::string& path) {
    std::string content;
    readFileToString(path, content);
    return content;
}

bool readFileToBytes(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for reading bytes: {}", path);
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    data.resize(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
        LGE_LOG_ERROR("Failed to read bytes from file: {}", path);
        return false;
    }
    return true;
}

std::vector<uint8_t> readFileToBytes(const std::string& path) {
    std::vector<uint8_t> data;
    readFileToBytes(path, data);
    return data;
}

bool readLines(const std::string& path, std::vector<std::string>& lines) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for reading lines: {}", path);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return true;
}

std::vector<std::string> readLines(const std::string& path) {
    std::vector<std::string> lines;
    readLines(path, lines);
    return lines;
}

bool writeStringToFile(const std::string& path, const std::string& content, bool append) {
    auto mode = append ? (std::ios::out | std::ios::app) : std::ios::out;
    std::ofstream file(path, mode);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for writing: {}", path);
        return false;
    }

    file << content;
    return file.good();
}

bool writeBytesToFile(const std::string& path, const std::vector<uint8_t>& data, bool append) {
    auto mode = std::ios::out | std::ios::binary;
    if (append) mode |= std::ios::app;
    std::ofstream file(path, mode);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for writing bytes: {}", path);
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

bool writeLinesToFile(const std::string& path, const std::vector<std::string>& lines, bool append) {
    auto mode = append ? (std::ios::out | std::ios::app) : std::ios::out;
    std::ofstream file(path, mode);
    if (!file.is_open()) {
        LGE_LOG_ERROR("Failed to open file for writing lines: {}", path);
        return false;
    }

    for (const auto& line : lines) {
        file << line << '\n';
    }
    return file.good();
}

void iterateDirectory(const std::string& path, std::function<void(const std::string&)> callback, bool recursive) {
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                callback(entry.path().string());
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                callback(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to iterate directory {}: {}", path, e.what());
    }
}

std::vector<std::string> listFiles(const std::string& path, bool recursive, bool includeDirectories) {
    std::vector<std::string> files;
    iterateDirectory(path, [&files, includeDirectories](const std::string& p) {
        if (includeDirectories || isFile(p)) {
            files.push_back(p);
        }
    }, recursive);
    return files;
}

std::vector<std::string> listFilesWithExtension(const std::string& path, const std::string& extension, bool recursive) {
    std::vector<std::string> files;
    iterateDirectory(path, [&files, &extension](const std::string& p) {
        if (isFile(p) && getFileExtension(p) == extension) {
            files.push_back(p);
        }
    }, recursive);
    return files;
}

bool isAbsolutePath(const std::string& path) {
    std::filesystem::path p(path);
    return p.is_absolute();
}

bool isRelativePath(const std::string& path) {
    return !isAbsolutePath(path);
}

bool isFile(const std::string& path) {
    return fileExists(path);
}

bool isDirectory(const std::string& path) {
    return directoryExists(path);
}

bool isSymLink(const std::string& path) {
    try {
        return std::filesystem::is_symlink(path);
    } catch (const std::exception& e) {
        return false;
    }
}

bool isReadOnly(const std::string& path) {
    try {
        auto perms = std::filesystem::status(path).permissions();
        return (perms & std::filesystem::perms::owner_write) == std::filesystem::perms::none;
    } catch (const std::exception& e) {
        return false;
    }
}

bool setReadOnly(const std::string& path, bool readOnly) {
    try {
        if (readOnly) {
            std::filesystem::permissions(path, std::filesystem::perms::owner_write, std::filesystem::perm_options::remove);
        } else {
            std::filesystem::permissions(path, std::filesystem::perms::owner_write, std::filesystem::perm_options::add);
        }
        return true;
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to set read-only status for {}: {}", path, e.what());
        return false;
    }
}

std::string getCurrentWorkingDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get current working directory: {}", e.what());
        return "";
    }
}

bool setCurrentWorkingDirectory(const std::string& path) {
    try {
        std::filesystem::current_path(path);
        return true;
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to set current working directory to {}: {}", path, e.what());
        return false;
    }
}

std::string getHomeDirectory() {
#ifdef _WIN32
    char* home = std::getenv("USERPROFILE");
#else
    char* home = std::getenv("HOME");
#endif
    return home ? std::string(home) : "";
}

std::string getTempDirectory() {
    try {
        return std::filesystem::temp_directory_path().string();
    } catch (const std::exception& e) {
        LGE_LOG_ERROR("Failed to get temp directory: {}", e.what());
        return "";
    }
}

std::string getTempFileName(const std::string& prefix, const std::string& extension) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static const char chars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);

    std::string tempDir = getTempDirectory();
    std::string random;
    for (int i = 0; i < 16; ++i) {
        random += chars[dis(gen)];
    }

    std::string fileName = prefix + random + extension;
    return joinPaths(tempDir, fileName);
}

FileWatcher::~FileWatcher() {
    clear();
}

void FileWatcher::watch(const std::string& path, bool recursive) {
    WatchEntry entry;
    entry.path = path;
    entry.recursive = recursive;
    try {
        entry.lastWriteTime = std::filesystem::last_write_time(path);
    } catch (...) {
        entry.lastWriteTime = std::filesystem::file_time_type{};
    }
    m_watches.push_back(entry);
}

void FileWatcher::unwatch(const std::string& path) {
    m_watches.erase(
        std::remove_if(m_watches.begin(), m_watches.end(),
            [&path](const WatchEntry& entry) { return entry.path == path; }),
        m_watches.end()
    );
}

void FileWatcher::clear() {
    m_watches.clear();
}

void FileWatcher::update() {
    for (auto& entry : m_watches) {
        try {
            auto currentTime = std::filesystem::last_write_time(entry.path);
            if (currentTime != entry.lastWriteTime) {
                entry.lastWriteTime = currentTime;
                if (m_callback) {
                    FileEvent event;
                    event.type = EventType::Modified;
                    event.path = entry.path;
                    m_callback(event);
                }
            }
        } catch (...) {
        }
    }
}

void FileWatcher::setCallback(std::function<void(const FileEvent&)> callback) {
    m_callback = std::move(callback);
}

MemoryMappedFile::MemoryMappedFile() = default;

MemoryMappedFile::~MemoryMappedFile() {
    close();
}

bool MemoryMappedFile::open(const std::string& path, bool readOnly) {
#ifdef _WIN32
    DWORD access = readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
    DWORD share = FILE_SHARE_READ;
    DWORD creation = OPEN_EXISTING;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    m_fileHandle = CreateFileA(path.c_str(), access, share, nullptr, creation, attributes, nullptr);
    if (m_fileHandle == INVALID_HANDLE_VALUE) {
        LGE_LOG_ERROR("Failed to open file for memory mapping: {}", path);
        return false;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(m_fileHandle, &fileSize)) {
        LGE_LOG_ERROR("Failed to get file size for memory mapping: {}", path);
        CloseHandle(m_fileHandle);
        m_fileHandle = nullptr;
        return false;
    }
    m_size = fileSize.QuadPart;

    DWORD protect = readOnly ? PAGE_READONLY : PAGE_READWRITE;
    m_mappingHandle = CreateFileMappingA(m_fileHandle, nullptr, protect, 0, 0, nullptr);
    if (!m_mappingHandle) {
        LGE_LOG_ERROR("Failed to create file mapping: {}", path);
        CloseHandle(m_fileHandle);
        m_fileHandle = nullptr;
        return false;
    }

    DWORD desiredAccess = readOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;
    m_data = MapViewOfFile(m_mappingHandle, desiredAccess, 0, 0, 0);
    if (!m_data) {
        LGE_LOG_ERROR("Failed to map view of file: {}", path);
        CloseHandle(m_mappingHandle);
        CloseHandle(m_fileHandle);
        m_mappingHandle = nullptr;
        m_fileHandle = nullptr;
        return false;
    }

    return true;
#else
    int flags = readOnly ? O_RDONLY : O_RDWR;
    int fd = ::open(path.c_str(), flags);
    if (fd == -1) {
        LGE_LOG_ERROR("Failed to open file for memory mapping: {}", path);
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        LGE_LOG_ERROR("Failed to get file size for memory mapping: {}", path);
        ::close(fd);
        return false;
    }
    m_size = st.st_size;

    int prot = readOnly ? PROT_READ : PROT_READ | PROT_WRITE;
    m_data = mmap(nullptr, m_size, prot, MAP_SHARED, fd, 0);
    if (m_data == MAP_FAILED) {
        LGE_LOG_ERROR("Failed to map file: {}", path);
        ::close(fd);
        m_data = nullptr;
        return false;
    }

    ::close(fd);
    m_fileHandle = reinterpret_cast<void*>(fd);
    return true;
#endif
}

void MemoryMappedFile::close() {
    if (m_data) {
#ifdef _WIN32
        UnmapViewOfFile(m_data);
#else
        munmap(m_data, m_size);
#endif
        m_data = nullptr;
    }
    if (m_mappingHandle) {
#ifdef _WIN32
        CloseHandle(m_mappingHandle);
#endif
        m_mappingHandle = nullptr;
    }
    if (m_fileHandle) {
#ifdef _WIN32
        CloseHandle(m_fileHandle);
#else
#endif
        m_fileHandle = nullptr;
    }
    m_size = 0;
}
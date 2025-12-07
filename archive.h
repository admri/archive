#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

#ifdef _WIN32
    #define DIR_SEP '\\'
#else
    #define DIR_SEP '/'
#endif

bool readBlock(FILE* file, char* buffer, size_t buffer_size, size_t* bytesRead);
bool writeBlock(FILE* file, const char* buffer, size_t bytes);
bool copyFileData(FILE* in, FILE* out, uint64_t fileSize);
char* getFileName(const char* filePath, bool stripExtension);

struct Archive
{
    const char* filePath;
    FILE* file;
    char** fileList;
    size_t fileCount;
};

struct ArchiveHeader
{
    char magic[4];
    uint16_t version;
    uint32_t fileCount;
    char reserved[20];
}; // 32 bytes

struct FileHeader
{
    uint16_t nameLength;
    uint64_t size;
}; // 10 bytes (+ variable-sized file name)

bool writeArchiveHeader(FILE* file, struct ArchiveHeader* header);
bool createArchive(struct Archive* archive, struct ArchiveHeader* header);

bool writeFileHeader(struct Archive* archive, const char* filePath, uint64_t fileSize);
bool addToArchive(struct Archive* archive, const char* filePath);

#endif // ARCHIVE_H
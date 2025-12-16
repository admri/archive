#include "file.h"

#include <zlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint16_t read_u16_le(const unsigned char b[2])
{
    return ((uint16_t)b[0] |
           ((uint16_t)b[1] << 8));
}

uint32_t read_u32_le(const unsigned char b[4])
{
    return ((uint32_t)b[0] |
           ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) |
           ((uint32_t)b[3] << 24));
}

uint64_t read_u64_le(const unsigned char b[8])
{
    uint64_t res = 0;
    for (int i = 0; i < 8; i++)
    {
        res |= (uint64_t)b[i] << (8 * i);
    }
    return res;
}

bool readFile(FILE* file, char* buffer, size_t buffer_size, size_t* bytesRead)
{
    if (!file || !buffer || !bytesRead) return false;

    clearerr(file);
    *bytesRead = fread(buffer, 1, buffer_size, file);

    if (ferror(file))
    {
        perror("Error reading file");
        return false;
    }

    return true;
}

bool writeFile(FILE* file, const char* buffer, size_t bytes)
{
    if (!file || !buffer) return false;

    size_t written = fwrite(buffer, 1, bytes, file);
    if (written != bytes)
    {
        perror("Error writing file");
        return false;
    }

    return true;
}

bool copyFileData(FILE* in, FILE* out, uint64_t fileSize)
{
    if (!in || !out) return false;

    unsigned char buffer[BUFFER_SIZE];
    uint64_t bytesLeft = fileSize;

    while (bytesLeft > 0)
    {
        size_t chunk = (bytesLeft < BUFFER_SIZE) ? (size_t)bytesLeft : BUFFER_SIZE;
        size_t readBytes;
        if (!readFile(in, (char*)buffer, chunk, &readBytes))
        {
            return false;
        }
        if (readBytes == 0)
        {
            fprintf(stderr, "Unexpected EOF while copying file data\n");
            return false;
        }
        if (!writeFile(out, (const char*)buffer, readBytes))
        {
            return false;
        }
        bytesLeft -= readBytes;
    }
    return true;
}

uint64_t getFileSize(FILE* file)
{
    if (!file) return 0;

    int64_t currentPos = ftell64(file);
    if (currentPos < 0) return 0;

    if (fseek64(file, 0, SEEK_END) != 0) return 0;

    int64_t size = ftell64(file);
    if (size < 0) return 0;

    if (fseek64(file, currentPos, SEEK_SET) != 0) return 0;

    return (uint64_t)size;
}

char* getFileName(const char* filePath, bool stripExtension)
{
    const char* separator = strrchr(filePath, DIR_SEP);
    const char* last = separator ? separator + 1 : filePath;

    size_t len = strlen(last);
    char* fileName = malloc(len + 1);
    if (!fileName) return NULL;

    strcpy(fileName, last);

    if (stripExtension)
    {
        char* dot = strrchr(fileName, '.');
        if (dot) *dot = '\0';
    }

    return fileName;
}

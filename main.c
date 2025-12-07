/* 
 * Archive is an archiver program which aims to make archiving files simple.
 * Copyright (C) 2025 Adam Riha
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 * For more information regarding this project please contact me at rihaadam1<at>seznam.cz.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

bool readBlock(FILE* file, char* buffer, size_t buffer_size, size_t* bytesRead)
{
    *bytesRead = fread(buffer, 1, buffer_size, file);
    if (*bytesRead == 0)
    {
        return false; // EOF
    }
    if (ferror(file))
    {
        perror("Error reading file");
        return false;
    }
    return true;
}

bool writeBlock(FILE* file, const char* buffer, size_t bytes)
{
    size_t written = fwrite(buffer, 1, bytes, file);
    if (written != bytes)
    {
        perror("Error writing file");
        return false;
    }
    return true;
}

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

bool writeArchiveHeader(FILE* file, struct ArchiveHeader* header)
{
    if (!writeBlock(file, header->magic, sizeof(header->magic)))
        return false;
    if (!writeBlock(file, (const char*)&header->version, sizeof(uint16_t)))
        return false;
    if (!writeBlock(file, (const char*)&header->fileCount, sizeof(uint32_t)))
        return false;
    if (!writeBlock(file, header->reserved, sizeof(header->reserved)))
        return false;

    return true;
}

bool createArchive(struct Archive* archive, struct ArchiveHeader* header)
{
    FILE* file = fopen(archive->filePath, "wb+");
    if (!file)
    {
        perror("Failed to open archive file");
        return false;
    }

    if (!writeArchiveHeader(file, header))
    {
        perror("Failed to write archive header");
        fclose(file);
        return false;
    }

    fseek(file, 0, SEEK_END);
    archive->file = file;

    return true;
}

bool writeFileHeader(struct Archive* archive, const char* filePath, uint64_t fileSize)
{
    const char* slash = strrchr(filePath, '/');      // Unix
    const char* backslash = strrchr(filePath, '\\'); // Windows
    const char* last = slash > backslash ? slash : backslash;
    const char* fileName = last ? last + 1 : filePath;

    size_t nameLength = strlen(fileName);
    
    struct FileHeader header = {
        .nameLength = (uint16_t)nameLength,
        .size = fileSize
    };

    if (!writeBlock(archive->file, (const char*)&header.nameLength, sizeof(uint16_t)))
        return false;
    if (!writeBlock(archive->file, (const char*)&header.size, sizeof(uint64_t)))
        return false;
    if (!writeBlock(archive->file, fileName, nameLength))
        return false;

    return true;
}

bool writeFileData(struct Archive* archive, FILE* file, uint64_t fileSize)
{
    char buffer[BUFFER_SIZE];
    uint64_t bytesLeft = fileSize;

    while (bytesLeft > 0)
    {
        size_t chunk = BUFFER_SIZE > bytesLeft ? BUFFER_SIZE : (size_t)bytesLeft;

        size_t read;
        if (!readBlock(file, buffer, chunk, &read))
        {
            perror("Failed to read file data");
            return false;
        }

        if (!writeBlock(archive->file, buffer, read))
        {
            perror("Failed to write file data");
            return false;            
        }

        bytesLeft -= read;
    }

    return true;
}

bool addToArchive(struct Archive* archive, const char* filePath)
{
    FILE* in = fopen(filePath, "rb");
    if (!in)
    {
        perror("Failed to open input file");
        return false;
    }
    
    fseek(in, 0, SEEK_END);
    uint64_t fileSize = ftell(in);
    fseek(in, 0, SEEK_SET);

    if (!writeFileHeader(archive, filePath, fileSize))
    {
        perror("Failed to write file header");
        fclose(in);
        return false;
    }

    if (!writeFileData(archive, in, fileSize))
    {
        perror("Failed to write file data");
        fclose(in);
        return false;
    }

    fclose(in);
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("Usage: %s [archive_name] [file1] [file2]...\n", argv[0]);
        return 1;
    }

    struct Archive archive = {
        .filePath = argv[1],
        .fileList = &argv[2],
        .fileCount = argc - 2
    };

    struct ArchiveHeader header = {
        .magic = "ARCH",
        .version = 1,
        .fileCount = 0,
        .reserved = {0}
    };

    if (!createArchive(&archive, &header))
    {
        perror("Failed to create archive");
        return 1;
    }
    
    for (size_t i = 0; i < archive.fileCount; i++)
    {
        if (addToArchive(&archive, archive.fileList[i]))
        {
            header.fileCount++;
            fseek(archive.file, 4 + sizeof(uint16_t), SEEK_SET);
            if (!writeBlock(archive.file, (const char*)&header.fileCount, sizeof(header.fileCount)))
            {
                perror("Failed to update file count in header");
                return 2;
            }
            fseek(archive.file, 0, SEEK_END);
        }
        else
        {
            printf("Failed to add file %zu to archive\n", i + 1);
        }
    }
    
    fclose(archive.file);

    return 0;
}

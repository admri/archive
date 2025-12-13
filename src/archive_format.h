#ifndef ARCHIVE_FORMAT_H
#define ARCHIVE_FORMAT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ARCHIVE_MAGIC "ARCH"
#define ARCHIVE_VERSION 1

typedef struct
{
    char magic[4];
    uint16_t version;
    uint32_t fileCount;
    char reserved[20];
} ArchiveHeader; // 32 bytes

ArchiveHeader* createArchiveHeader();
void freeArchiveHeader(ArchiveHeader* header);

bool writeArchiveHeader(FILE* file, ArchiveHeader* header);
bool updateArchiveHeaderFileCount(FILE* file, uint32_t fileCount);
bool readArchiveHeader(FILE* file, ArchiveHeader* header);

#endif // ARCHIVE_FORMAT_H

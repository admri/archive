#ifndef ARCHIVE_INTERNAL_H
#define ARCHIVE_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct Archive
{
    const char* filePath;
    FILE* file;
    size_t fileCount;
    size_t currentFileIndex;
} Archive;

Archive* createArchive(const char* path, const char* fileMode);
void freeArchive(Archive* archive);

#endif // ARCHIVE_INTERNAL_H

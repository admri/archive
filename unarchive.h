#ifndef UNARCHIVE_H
#define UNARCHIVE_H

#include "archive.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

bool readArchiveHeader(FILE* file, struct ArchiveHeader* header);

bool readFileHeader(FILE* archiveFile, struct FileHeader* header, char** fileName);
bool writeFile(FILE* archiveFile, const char* dirPath);

bool unarchive(const char* filePath);

#endif // UNARCHIVE_H
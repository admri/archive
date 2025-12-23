#ifndef ARCHIVE_HEADER_H
#define ARCHIVE_HEADER_H

#include <arch/arch_types.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool createArchiveHeader(ArchiveHeader* header);
void freeArchiveHeader(ArchiveHeader* header);

bool writeArchiveHeader(FILE* file, const ArchiveHeader* header);
bool updateArchiveHeaderFileCount(FILE* file, uint32_t fileCount);
bool readArchiveHeader(FILE* file, ArchiveHeader* header);

#endif // ARCHIVE_HEADER_H

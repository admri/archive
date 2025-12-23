#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <arch/arch_types.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool createFileHeader(const char* path, uint8_t flags, FileHeader* header, FILE** outFile, uint64_t* outOrigSize);
void freeFileHeader(FileHeader* header);

bool writeFileHeader(FILE* file, const FileHeader* header, const char* fileName, uint64_t* outCompSizePos, uint64_t* outCrcUncompressedPos, uint64_t* outCrcCompressedPos);

bool updateFileHeaderCompSize(FileHeader* header, FILE* file, uint64_t compSizePos, uint64_t compSize);
bool updateFileHeaderCRC32(FileHeader* header, FILE* file, uint64_t crc32UncompressedPos, uint64_t crc32CompressedPos, uint32_t crc32Uncompressed, uint32_t crc32Compressed);

bool readFileHeader(FILE* archiveFile, FileHeader* header, char** fileName);

#endif // FILE_HEADER_H

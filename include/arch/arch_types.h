#ifndef ARCHIVE_TYPES_H
#define ARCHIVE_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Archive format constants ===== */

#define ARCH_MAGIC 0x48435241u  /* "ARCH" */
#define ARCH_FILE_MAGIC 0x454C4946u  /* "FILE" */

#define ARCH_VERSION 1

/* ===== Flags ===== */

#define ARCH_FLAG_COMPRESSED 0x01

/* ===== Archive Header ===== */

typedef struct ArchiveHeader
{
    uint32_t magic;
    uint16_t version;
    uint32_t fileCount;
    char reserved[20];
} ArchiveHeader; // 32 bytes

/* ===== File Header ===== */

typedef struct FileHeader
{
    uint32_t magic;
    uint16_t nameLength;
    uint64_t origSize;
    uint64_t compSize;
    uint32_t crc32_uncompressed;
    uint32_t crc32_compressed;
    uint8_t flags;
} FileHeader; // 31 bytes (+ variable-sized file name)

#ifdef __cplusplus
}
#endif

#endif // ARCHIVE_TYPES_H

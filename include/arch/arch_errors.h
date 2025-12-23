#ifndef ARCH_ERRORS_H
#define ARCH_ERRORS_H

#include <arch/arch_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Error codes ===== */

typedef enum ArchResult
{
    ARCH_OK = 0,

    ARCH_ERR_INVALID_ARGUMENT,
    ARCH_ERR_IO,
    ARCH_ERR_NOT_AN_ARCHIVE,
    ARCH_ERR_UNSUPPORTED_VERSION,
    ARCH_ERR_CORRUPTED,
    ARCH_ERR_OUT_OF_MEMORY,
    ARCH_ERR_COMPRESSION,
    ARCH_ERR_INTERNAL

} ArchResult;

const char* arch_strerror(ArchResult err);

#ifdef __cplusplus
}
#endif

#endif // ARCH_ERRORS_H

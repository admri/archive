#include <arch/arch_errors.h>

const char* arch_strerror(ArchResult err)
{
    switch (err)
    {
        case ARCH_OK:
            return "No error";

        case ARCH_ERR_INVALID_ARGUMENT:
            return "Invalid argument";

        case ARCH_ERR_IO:
            return "I/O error";

        case ARCH_ERR_NOT_AN_ARCHIVE:
            return "Unknown archive file type";

        case ARCH_ERR_OUT_OF_MEMORY:
            return "Out of memory";

        case ARCH_ERR_COMPRESSION:
            return "Compression or decompression failed";

        case ARCH_ERR_CORRUPTED:
            return "Archive is corrupted";

        case ARCH_ERR_UNSUPPORTED_VERSION:
            return "Unsupported archive feature";

        case ARCH_ERR_INTERNAL:
            return "Internal library error";

        default:
            return "Unknown archiver error";
    }
}

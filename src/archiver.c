#include <arch/archiver.h>

#include "core/archive.h"
#include "core/archive_header.h"
#include "core/file_header.h"
#include "util/file.h"

Archive* arch_create(const char *path)
{
    if (!path) return NULL;

    Archive* archive = NULL;
    ArchiveHeader* header = NULL;

    archive = createArchive(path, "wb+");
    if (!archive) return NULL;

    header = createArchiveHeader();
    if (!header) goto cleanup;

    if (!writeArchiveHeader(archive->file, header)) goto cleanup;

    freeArchiveHeader(header);
    return archive;

cleanup:
    freeArchive(archive);
    freeArchiveHeader(header);
    return NULL;
}

bool arch_addFile(Archive* archive, const char* path)
{
    if (!archive || !path) return false;

    FILE* file = NULL;
    FileHeader* fileHeader = NULL;
    char* fileName = NULL;
    uint64_t compSizePos = 0;
    uint64_t compSize = 0;
    bool success = false;

    fileHeader = createFileHeader(path, COMPRESSED_FLAG, &file);
    if (!fileHeader) return false;

    fileName = getFileName(path, false);
    if (!fileName) goto cleanup;
    if (!writeFileHeader(archive->file, fileHeader, fileName, &compSizePos)) goto cleanup;

    if (!compressFileStream(file, archive->file, &compSize)) goto cleanup;

    if (!updateFileHeaderCompSize(fileHeader, archive->file, compSizePos, compSize)) goto cleanup;

    archive->fileCount++;
    if (!updateArchiveHeaderFileCount(archive->file, archive->fileCount)) goto cleanup;

    success = true;

cleanup:
    freeFileHeader(fileHeader);
    free(fileName);
    if (file) fclose(file);
    return success;
}

void arch_close(Archive* archive)
{
    if (archive) freeArchive(archive);
}

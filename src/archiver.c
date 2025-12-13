#include <archiver/archiver.h>

#include "archive_internal.h"
#include "archive_format.h"
#include "archive_file.h"

Archive* archive_create(const char *path)
{
    if (!path) return NULL;

    Archive* archive;
    ArchiveHeader* header;

    archive = createArchive(path);
    if (!archive) goto cleanup;

    header = createArchiveHeader();
    if (!header) goto cleanup;

    if (!writeArchiveHeader(archive->file, header)) goto cleanup;

    return archive;

cleanup:
    freeArchive(archive);
    freeArchiveHeader(header);
    return NULL;
}

bool archive_add_file(Archive* archive, const char* path)
{
    if (!archive || !path) return false;

    FILE* file = NULL;
    FileHeader* fileHeader = NULL;
    uint64_t compSizePos = 0;
    uint64_t compSize = 0;

    fileHeader = createFileHeader(path, COMPRESSED_FLAG, &file);
    if (!fileHeader) goto cleanup;

    if (!writeFileHeader(archive->file, fileHeader, path, &compSizePos)) goto cleanup;

    if (!compressFileStream(file, archive->file, &compSize)) goto cleanup;

    if (!updateFileHeaderCompSize(fileHeader, archive->file, compSizePos, compSize)) goto cleanup;

    archive->fileCount++;
    if (!updateArchiveHeaderFileCount(archive->file, archive->fileCount)) goto cleanup;

    return true;

cleanup:
    freeFileHeader(fileHeader);
    if (file) fclose(file);
    return false;
}

void archive_close(Archive* archive)
{
    if (archive) freeArchive(archive);
}

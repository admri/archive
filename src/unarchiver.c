#include <arch/unarchiver.h>

#include "core/archive.h"
#include "core/archive_header.h"
#include "core/file_header.h"
#include "util/file.h"

#include <stdlib.h>
#include <stdio.h>

Archive* arch_open(const char* path)
{
    if (!path) return NULL;

    Archive* archive = NULL;
    ArchiveHeader* header = NULL;

    archive = createArchive(path, "rb");
    if (!archive) return NULL;

    header = readArchiveHeader(archive->file);
    if (!header)
    {
        freeArchive(archive);
        return NULL;
    }

    archive->fileCount = header->fileCount;
    freeArchiveHeader(header);
    
    return archive;
}

bool arch_retrieveNextFile(Archive* archive, const char* output_dir)
{
    if (!archive || !output_dir) return false;
    if (archive->currentFileIndex >= archive->fileCount) return false;
    
    FileHeader* header = NULL;
    char* fileName = NULL;
    char* filePath = NULL;
    FILE* file = NULL;
    bool ok = false;

    header = (FileHeader*)malloc(sizeof(FileHeader));
    if (!header) return false;
    if (!readFileHeader(archive->file, header, &fileName)) goto cleanup;

    size_t filePathSize = strlen(output_dir) + sizeof(DIR_SEP) + strlen(fileName) + 1;
    filePath = malloc(filePathSize);
    if (!filePath) goto cleanup;

    snprintf(filePath, filePathSize, "%s%c%s", output_dir, DIR_SEP, fileName);

    file = fopen(filePath, "wb");
    if (!file)
    {
        perror("Failed to open output file");
        goto cleanup;
    }

    if (header->flags & COMPRESSED_FLAG)
    {
        ok = decompressFileStream(archive->file, file, header->compSize);
    }
    else
    {
        ok = copyFileData(archive->file, file, header->origSize);
    }

cleanup:
    if (file) fclose(file);
    free(filePath);
    free(fileName);
    freeFileHeader(header);

    archive->currentFileIndex++;
    return ok;
}

size_t arch_getFileCount(Archive *archive)
{
    if (!archive) return 0;
    return archive->fileCount;
}

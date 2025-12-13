#include <archiver/unarchiver.h>

#include "archive_internal.h"
#include "archive_format.h"
#include "archive_file.h"
#include "file.h"

Archive* archive_open(const char* path)
{
    if (!path) return NULL;

    Archive* archive = createArchive(path);
    if (!archive) return NULL;

    return archive;
}

bool archive_retrieve_next_file(Archive* archive, const char* output_dir)
{
    if (!archive || !output_dir) return false;
    if (archive->currentFileIndex >= archive->fileCount) return false;
    
    FileHeader* header = NULL;
    char* fileName = NULL;
    char* filePath = NULL;
    FILE* file = NULL;
    bool ok = false;

    if (!readFileHeader(archive->file, &header, &fileName)) goto cleanup;

    size_t filePathSize = strlen(output_dir) + sizeof(DIR_SEP) + strlen(fileName) + 1;
    filePath = malloc(filePathSize);
    if (!filePath) goto cleanup;

    file = fopen(filePath, "wb");
    if (!file) goto cleanup;

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

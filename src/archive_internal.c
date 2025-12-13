#include "archive_internal.h"

Archive* createArchive(const char* path)
{
    Archive* archive = malloc(sizeof *archive);
    if (!archive) return false;

    archive->filePath = strdup(path);
    if (!archive->filePath)
    {
        free(archive);
        return false;
    }

    archive->file = fopen(path, "wb+");
    if (!archive->file)
    {
        free((char*)archive->filePath);
        free(archive);
        return false;
    }

    archive->fileCount = 0;

    return true;
}

void freeArchive(Archive *archive)
{
    if (!archive) return;

    if (archive->file)
    {
        fflush(archive->file);
        fclose(archive->file);
    }
    if (archive->filePath)
    {
        free((char*)archive->filePath);
    }
    free(archive);
}

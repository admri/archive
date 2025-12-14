#include <archiver/archiver.h>
#include <archiver/unarchiver.h>

#include "../src/file.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: arch [archive_name] [file1] [file2]...\n");
        return 1;
    }

    const char* archiveFilePath = argv[1];
    Archive* archive = NULL;

    if (argc > 2)
    {
        int fileCount = argc - 2;
        const char** filePaths = (const char**)&argv[2];
        
        printf("Creating archive: %s\n", archiveFilePath);
        archive = archiver_create(archiveFilePath);
        if (!archive)
        {
            printf("Failed to create archive: %s\n", archiveFilePath);
            return 1;
        }

        for (int i = 0; i < fileCount; ++i)
        {
            printf("Adding file to archive: %s\n", filePaths[i]);
            if (!archiver_addFile(archive, filePaths[i]))
            {
                printf("Failed to add file to archive: %s\n", filePaths[i]);
            }
        }
    }
    else
    {
        archive = archiver_open(archiveFilePath);
        if (!archive)
        {
            printf("Failed to open archive: %s\n", archiveFilePath);
            return 1;
        }

        const char* outputDir = getFileName(archiveFilePath, true);
        if (MKDIR(outputDir) != 0)
        {
            perror("Failed to create output directory");
            archiver_close(archive);
            return 1;
        }

        printf("Extracting files from archive: %s\n", archiveFilePath);
        for (size_t i = 1; i <= archiver_getFileCount(archive); ++i)
        {
            printf("Extracting file %zu to directory: %s\n", i, outputDir);
            if (!archiver_retrieveNextFile(archive, outputDir))
            {
                printf("Failed to extract file %zu from archive\n", i);
            }
        }
    }

    archiver_close(archive);

    return 0;
}
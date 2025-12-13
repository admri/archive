#include <archiver/archiver.h>
#include <archiver/unarchiver.h>

#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: arch [archive_name] [file1] [file2]...\n");
        return 1;
    }

    const char* archiveFilePath = argv[1];
    int fileCount = argc - 2;
    const char** filePaths = (const char**)&argv[2];
    
    Archive* archive = archive_create(archiveFilePath);
    if (!archive)
    {
        printf("Failed to create archive: %s\n", archiveFilePath);
        return 1;
    }

    for (int i = 0; i < fileCount; ++i)
    {
        if (!archive_add_file(archive, filePaths[i]))
        {
            printf("Failed to add file to archive: %s\n", filePaths[i]);
        }
    }

    archive_close(archive);

    return 0;
}
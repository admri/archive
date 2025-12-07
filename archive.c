#include "archive.h"

bool readBlock(FILE* file, char* buffer, size_t buffer_size, size_t* bytesRead)
{
    *bytesRead = fread(buffer, 1, buffer_size, file);
    if (*bytesRead == 0)
    {
        return false; // EOF
    }
    if (ferror(file))
    {
        perror("Error reading file");
        return false;
    }
    return true;
}

bool writeBlock(FILE* file, const char* buffer, size_t bytes)
{
    size_t written = fwrite(buffer, 1, bytes, file);
    if (written != bytes)
    {
        perror("Error writing file");
        return false;
    }
    return true;
}

bool copyFileData(FILE* in, FILE* out, uint64_t fileSize)
{
    char buffer[BUFFER_SIZE];
    uint64_t bytesLeft = fileSize;

    while (bytesLeft > 0)
    {
        size_t chunk = BUFFER_SIZE < bytesLeft ? BUFFER_SIZE : (size_t)bytesLeft;

        size_t read;
        if (!readBlock(in, buffer, chunk, &read))
        {
            perror("Failed to read file data");
            return false;
        }

        if (!writeBlock(out, buffer, read))
        {
            perror("Failed to write file data");
            return false;            
        }

        bytesLeft -= read;
    }

    return true;
}

char* getFileName(const char* filePath, bool stripExtension)
{
    const char* separator = strrchr(filePath, DIR_SEP);
    const char* last = separator ? separator + 1 : filePath;

    size_t len = strlen(last);
    char* fileName = malloc(len + 1);
    if (!fileName) return NULL;

    strcpy(fileName, last);

    if (stripExtension)
    {
        char* dot = strrchr(fileName, '.');
        if (dot) *dot = '\0';
    }

    return fileName;
}

bool writeArchiveHeader(FILE* file, struct ArchiveHeader* header)
{
    if (!writeBlock(file, header->magic, sizeof(header->magic)))
        return false;
    if (!writeBlock(file, (const char*)&header->version, sizeof(uint16_t)))
        return false;
    if (!writeBlock(file, (const char*)&header->fileCount, sizeof(uint32_t)))
        return false;
    if (!writeBlock(file, header->reserved, sizeof(header->reserved)))
        return false;

    return true;
}

bool createArchive(struct Archive* archive, struct ArchiveHeader* header)
{
    FILE* file = fopen(archive->filePath, "wb+");
    if (!file)
    {
        perror("Failed to open archive file");
        return false;
    }

    if (!writeArchiveHeader(file, header))
    {
        perror("Failed to write archive header");
        fclose(file);
        return false;
    }

    fseek(file, 0, SEEK_END);
    archive->file = file;

    return true;
}

bool writeFileHeader(struct Archive* archive, const char* filePath, uint64_t fileSize)
{
    char* fileName = getFileName(filePath, false);
    size_t nameLength = strlen(fileName);
    
    struct FileHeader header = {
        .nameLength = (uint16_t)nameLength,
        .size = fileSize
    };

    if (!writeBlock(archive->file, (const char*)&header.nameLength, sizeof(uint16_t)))
    {
        free(fileName);
        return false;
    }
    if (!writeBlock(archive->file, (const char*)&header.size, sizeof(uint64_t)))
    {
        free(fileName);
        return false;
    }
    if (!writeBlock(archive->file, fileName, nameLength))
    {
        free(fileName);
        return false;
    }

    free(fileName);
    return true;
}

bool addToArchive(struct Archive* archive, const char* filePath)
{
    FILE* in = fopen(filePath, "rb");
    if (!in)
    {
        perror("Failed to open input file");
        return false;
    }
    
    fseek(in, 0, SEEK_END);
    uint64_t fileSize = ftell(in);
    fseek(in, 0, SEEK_SET);

    if (!writeFileHeader(archive, filePath, fileSize))
    {
        perror("Failed to write file header");
        fclose(in);
        return false;
    }

    if (!copyFileData(in, archive->file, fileSize))
    {
        perror("Failed to write file data");
        fclose(in);
        return false;
    }

    fclose(in);
    return true;
}
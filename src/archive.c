#include "archive.h"

bool writeArchiveHeader(FILE* file, struct ArchiveHeader* header)
{
    if (!writeFile(file, header->magic, sizeof(header->magic)))
        return false;

    if (!writeFile(file, (const char*)&header->version, sizeof(uint16_t)))
        return false;

    if (!writeFile(file, (const char*)&header->fileCount, sizeof(uint32_t)))
        return false;

    if (!writeFile(file, header->reserved, sizeof(header->reserved)))
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

    archive->file = file;

    return true;
}

bool writeFileHeader(FILE* file, struct FileHeader* header, const char* fileName, long* compSizePos)
{
    if (!writeFile(file, (const char*)&header->nameLength, sizeof(uint16_t)))
        return false;

    if (!writeFile(file, (const char*)&header->origSize, sizeof(uint64_t)))
        return false;

    *compSizePos = ftell(file);
    if (!writeFile(file, (const char*)&header->compSize, sizeof(uint64_t)))
        return false;

    if (!writeFile(file, (const char*)&header->flags, sizeof(uint8_t)))
        return false;

    if (!writeFile(file, fileName, header->nameLength))
        return false;

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
    uint64_t origSize = ftell(in);
    fseek(in, 0, SEEK_SET);

    char* fileName = getFileName(filePath, false);
    size_t nameLen = strlen(fileName);

    struct FileHeader header = {
        .nameLength = (uint16_t)nameLen,
        .origSize = origSize,
        .compSize = 0,
        .flags = COMPRESSED_FLAG
    };

    long compSizePos;
    if (!writeFileHeader(archive->file, &header, fileName, &compSizePos))
    {
        perror("Failed to write file header");
        fclose(in);
        return false;
    }

    free(fileName);

    uint64_t compSize;
    if (!compressFileStream(in, archive->file, &compSize))
    {
        perror("Failed to compress file data");
        fclose(in);
        return false;
    }

    fflush(archive->file);
    fseek(archive->file, compSizePos, SEEK_SET);
    if (!writeFile(archive->file, (const char*)&compSize, sizeof(uint64_t)))
    {
        perror("Failed to update compressed size in header");
        fclose(in);
        return false;
    }
    fflush(archive->file);
    fseek(archive->file, 0, SEEK_END);

    fclose(in);
    return true;
}

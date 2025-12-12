#include "file.h"

#include <zlib.h>

bool readFile(FILE* file, char* buffer, size_t buffer_size, size_t* bytesRead)
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

bool writeFile(FILE* file, const char* buffer, size_t bytes)
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
        if (!readFile(in, buffer, chunk, &read))
        {
            perror("Failed to read file data");
            return false;
        }

        if (!writeFile(out, buffer, read))
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

bool compressFileStream(FILE* inFile, FILE* outFile, uint64_t* compSize)
{
    unsigned char in[BUFFER_SIZE];
    unsigned char out[BUFFER_SIZE];
    size_t totalWritten = 0;

    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        return false;
    }

    int flush;
    size_t readBytes;
    do
    {
        readBytes = fread(in, 1, BUFFER_SIZE, inFile);
        flush = feof(inFile) ? Z_FINISH : Z_NO_FLUSH;

        strm.next_in = in;
        strm.avail_in = readBytes;

        do
        {
            strm.next_out = out;
            strm.avail_out = BUFFER_SIZE;

            deflate(&strm, flush);

            size_t have = BUFFER_SIZE - strm.avail_out;
            if (!writeFile(outFile, (const char*)out, have))
            {
                deflateEnd(&strm);
                return false;
            }
            totalWritten += have;
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&strm);
    *compSize = totalWritten;
    return true;
}

bool decompressFileStream(FILE* inFile, FILE* outFile, uint64_t compSize)
{
    unsigned char in[BUFFER_SIZE];
    unsigned char out[BUFFER_SIZE];
    size_t totalRead = 0;

    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    if (inflateInit(&strm) != Z_OK)
    {
        return false;
    }

    int ret;
    do
    {
        size_t toRead = BUFFER_SIZE;
        if (compSize - totalRead < BUFFER_SIZE) toRead = (size_t)(compSize - totalRead);
        size_t bytesRead;
        readFile(inFile, in, toRead, &bytesRead);
        if (bytesRead == 0) break;

        totalRead += bytesRead;

        strm.next_in = in;
        strm.avail_in = bytesRead;

        do
        {
            strm.next_out = out;
            strm.avail_out = BUFFER_SIZE;

            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&strm);
                return false;
            }

            size_t have = BUFFER_SIZE - strm.avail_out;
            if (!writeFile(outFile, (const char*)out, have))
            {
                inflateEnd(&strm);
                return false;
            }
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END && totalRead < compSize);

    inflateEnd(&strm);
    return true;
}

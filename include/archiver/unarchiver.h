#ifndef UNARCHIVER_H
#define UNARCHIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* archiver_open(const char* path);
bool archiver_retrieveNextFile(Archive* archive, const char* output_dir);

size_t archiver_getFileCount(Archive* archive);

#ifdef __cplusplus
}
#endif

#endif // UNARCHIVER_H

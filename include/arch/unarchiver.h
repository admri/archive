#ifndef UNARCHIVER_H
#define UNARCHIVER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* arch_open(const char* path);
bool arch_retrieveNextFile(Archive* archive, const char* output_dir);

size_t arch_getFileCount(Archive* archive);

#ifdef __cplusplus
}
#endif

#endif // UNARCHIVER_H

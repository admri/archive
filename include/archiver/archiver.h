#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* archiver_create(const char* path);
bool archiver_addFile(struct Archive* archive, const char* path);
void archiver_close(struct Archive* archive);

#ifdef __cplusplus
}
#endif

#endif // ARCHIVER_H

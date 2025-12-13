#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* archive_create(const char* path);
bool archive_add_file(struct Archive* archive, const char* path);
void archive_close(struct Archive* archive);

#ifdef __cplusplus
}
#endif

#endif // ARCHIVER_H

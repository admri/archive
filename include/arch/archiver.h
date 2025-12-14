#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* arch_create(const char* path);
bool arch_addFile(struct Archive* archive, const char* path);
void arch_close(struct Archive* archive);

#ifdef __cplusplus
}
#endif

#endif // ARCHIVER_H

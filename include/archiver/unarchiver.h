#ifndef UNARCHIVER_H
#define UNARCHIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Archive Archive;

Archive* archive_open(const char* path);
bool archive_retrieve_next_file(Archive* archive, const char* output_dir);

#ifdef __cplusplus
}
#endif

#endif // UNARCHIVER_H

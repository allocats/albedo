#include "buffers.h"

#include "../diagnostics/diagnostics.h"
#include "types.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef DEBUG_MODE
#include <stdio.h>
#endif /* ifdef DEBUG_MOD */

extern AlbedoCtx albedo_ctx;

void map_file(const char* path) {
    if (albedo_ctx.file_count >= albedo_ctx.file_capacity) {
        usize size = albedo_ctx.file_capacity * sizeof(FileBuffer);

        albedo_ctx.file_capacity *= FB_GROWTH_FACTOR;
        albedo_ctx.files = arena_realloc(
            albedo_ctx.arena,
            albedo_ctx.files,
            size,
            size * FB_GROWTH_FACTOR
        );
    }

    i32 fd = open(path, O_RDONLY);
    if (fd == -1) {
        if (errno == EBADF) {
            err_file_not_found(path);
        } else {
            err_cant_open_file(path);
        }

        return;
    }

    struct stat st;
    if (fstat(fd, &st) != 0) {
        if (errno == EBADF) {
            err_file_not_found(path);
        } else {
            err_cant_open_file(path);
        }

        close(fd);
        return;
    }

    usize len = st.st_size + 1;

    char* buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    if (buffer == MAP_FAILED) {
        err_cant_map_file(path);
        close(fd);
        return;
    }

    buffer[len - 1] = 0;

    FileBuffer* fb = &albedo_ctx.files[albedo_ctx.file_count++];

    fb -> path = path; 
    fb -> buffer = buffer; 
    fb -> needs_free = true; 
    fb -> size = len;

    #ifdef DEBUG_MODE
    printf("Mapped file: %s\n", path);
    #endif /* ifdef DEBUG_MODE */
}

void buffer_cleanup() {
    for (u32 i = 0; i < albedo_ctx.file_count; i++) {
        FileBuffer* file = &albedo_ctx.files[i];

        if (file -> needs_free) {
            munmap(file -> buffer, file -> size); 
            file -> needs_free = false;
        }
    }
}

void file_buffers_append(char* paths[], usize count) {
    for (usize i = 0; i < count; i++) {
        map_file(paths[i]);
    }
}

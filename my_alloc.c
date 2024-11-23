#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MEMORY_SIZE 1024 * 1024
#define MEMORY_CHUNKS_CAP 1024

// mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);

// [                         ]
// [       ][//][  ][//]     ]

typedef struct  {
    char* start;
    size_t chunkSize;
} MemoryChunk;

typedef struct {
    size_t count;
    MemoryChunk chunks[MEMORY_CHUNKS_CAP];
} MemoryChuck_List;

char heap[MEMORY_SIZE];

MemoryChuck_List freeChunks = {
    .count = 1,
    .chunks = {
        [0] = {.start = heap, .chunkSize = sizeof(heap)},
    },
};
MemoryChuck_List reservedChunks = {0};
MemoryChuck_List tmpChunks = {0};

int chunkListFind(MemoryChuck_List *list, void* ptr) {
    for (size_t i = 0; i < reservedChunks.count; ++i) {
        if (reservedChunks.chunks[i].start == ptr) {
            return (int) i;
        }
    }
    return -1;
}

void mergeFreeChunks() {
    assert(0 && "TODO: merge free chunks\n");
}

void chunkRemove(MemoryChuck_List *list, size_t index) {
    assert(index < list->count);

    for (size_t i = index; i < list->count - 1; ++i) {
        list->chunks[i] = list->chunks[i + 1];
    }

    list->count -= 1;
}

void chunkInsert(MemoryChuck_List *list, void* start, size_t size) {
    assert(list->count < MEMORY_CHUNKS_CAP);
    list->chunks[list->count].start = start;
    list->chunks[list->count].chunkSize = size;

    for (size_t i = list->count; i > 0 && list->chunks[i].start < list->chunks[i - 1].start; --i) {
        const MemoryChunk tmp = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = tmp;
    }

    list->count += 1;
}

void chunkPrintList(const MemoryChuck_List *list) {
    printf("Chunk: (%zu):\n", list->count);
    for (size_t i = 0; i < list->count; ++i) {
        printf("    start: %p, size: %zu\n", list->chunks[i].start, list->chunks[i].chunkSize);
    }
}

void *my_alloc(size_t chunkSize) {
    assert(chunkSize > 0);

    for (size_t i = 0; i < freeChunks.count; ++i) {
        const MemoryChunk chunk = freeChunks.chunks[i];

        if (chunk.chunkSize >= chunkSize) {
            chunkRemove(&freeChunks, i);

            const size_t restSize = chunk.chunkSize - chunkSize;
            chunkInsert(&reservedChunks, chunk.start, chunkSize);
            if (restSize > 0) {
                chunkInsert(&freeChunks, chunk.start + chunkSize, restSize);
            }

            return chunk.start;
        }
    }

    assert(0 && "The chunk is too large\n");
}

void my_free(void* ptr) {
    assert(ptr != NULL && "Trying to deallocate free memory!\n");
    const int index = chunkListFind(&reservedChunks, ptr);
    assert(index >= 0);
    assert(ptr == reservedChunks.chunks[index].start);

    chunkInsert(&freeChunks, reservedChunks.chunks[index].start, reservedChunks.chunks[index].chunkSize);
    chunkRemove(&reservedChunks, (size_t) index);
}

int main() {
    printf("Free chunks:\n");
    chunkPrintList(&freeChunks);
    printf("Reserved chunks:\n");
    chunkPrintList(&reservedChunks);
    int *intPtr = (int*)my_alloc(sizeof(int));

    *intPtr = 123;
    int a = 1;
    // my_free(&a);
    printf("Free chunks:\n");
    chunkPrintList(&freeChunks);
    printf("Reserved chunks:\n");
    chunkPrintList(&reservedChunks);

    printf("%d\n", *intPtr);
    my_free(intPtr);
    printf("Free chunks:\n");
    chunkPrintList(&freeChunks);
    printf("Reserved chunks:\n");
    chunkPrintList(&reservedChunks);
    int *something = (int*)my_alloc(sizeof(int));
    *something = 567;
    printf("%d\n", *intPtr);

    return 0;
}

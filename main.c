#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct {
    char* start;
    int offset;
    unsigned int size;
    // NOTE c doesn't have vectors..., should i build one ?
    //https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
    //int* trackers[];
} StackAlloc;

StackAlloc stack_alloc_build(int size) {
    char* chunk = malloc(size);
    memset(chunk, 0, size);

    StackAlloc stack_alloc = {.start = chunk, .size = size, .offset = 0};

    return stack_alloc;
}

void *stack_alloc_add(StackAlloc* alloc, unsigned int added_size) {
    if ((unsigned int)alloc->size - alloc->offset < added_size) {
        printf("[WARINING] added size is bigger then allocator size\n");
        return NULL;
    }

    alloc->offset += added_size;
    char* tracker = alloc->start + alloc->offset;
    alloc->start += alloc->offset;

    return tracker;
}

// NOTE in order to pop, need to store prev trackers...
void stack_alloc_pop(StackAlloc* alloc) {
    if (alloc->size <= 0u) {
        printf("[WARNING] can't pop, arena is empty!\n");
        return;
    }
    // TODO reduce offset (total of used bytes)
}

void stack_alloc_free (StackAlloc* alloc) {
    free(alloc->start);
}

typedef struct {
    int x;
    int y;
}Point;

int main () {
    #define SIZE 1000
    StackAlloc stack_alloc = stack_alloc_build(SIZE);

    const char* string = "Hello world";
    stack_alloc_add(&stack_alloc, strlen(string)+1);
    Point* point = (Point*)stack_alloc_add(&stack_alloc, sizeof(Point));
    point->x = 100;
    point->y = -100;

    printf("x: %d\n",point->x);
    printf("y: %d\n",point->y);

    stack_alloc_free(&stack_alloc);
}

/*
- code review time
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define DEFAULT_CAPICITY 256

typedef struct {
    size_t  *items;
    // needed for malloc
    size_t  capacity;// nums of max elements
    // counts needed if i ran out of cap
    size_t  count;
}VecSizeT;

typedef struct {
    char* start;
    size_t offset;
    size_t size;
    // can't use an array cuz dync one can grow, the other can't
    VecSizeT prev_allocations;

} StackAlloc;

void vec_size_t_push (VecSizeT* vec, size_t item) {
    if (vec->capacity <= vec->count) {
        if (vec->capacity == 0) vec->capacity = DEFAULT_CAPICITY;
        else vec->capacity *= 2;
        // NOTE chunk will contain garbage, can't use memset cuz
        //it will init first bytes(want to to init after items)
        vec->items = realloc(vec->items, vec->capacity*sizeof(size_t));
    }

    vec->items[vec->count++] = item;
}

void vec_size_t_pop (VecSizeT* vec) {
    if (vec->count == 0) {
        printf("[WARNING] VecSizeT is empty");
        return;
    }

    vec->items[vec->count--] = 0u;
}


StackAlloc stack_alloc_build(int size) {
    char* chunk = malloc(size);
    memset(chunk, 0, size);

    StackAlloc stack_alloc = {.start = chunk, .size = size, .offset = 0u, .prev_allocations = {0}};

    return stack_alloc;
}

void *stack_alloc_add(StackAlloc* alloc, size_t added_size) {

    if (alloc->size - alloc->offset < added_size) {
        printf("[WARINING] added size is bigger then allocator size\n");
        return NULL;
    }

    size_t offset_before = alloc->offset;
    alloc->offset += added_size;

    vec_size_t_push(&alloc->prev_allocations, added_size);

    printf("[iNFO] Push used_bytes: %zu -> %zu\n", offset_before, alloc->offset);

    char* tracker = alloc->start + alloc->offset;
    return tracker;
}

// NOTE in order to pop, need to store allocation sizes
void stack_alloc_pop(StackAlloc* alloc) {
    if (alloc->size <= 0) {
        printf("[WARNING] can't pop, arena is empty!\n");
        return;
    }

    size_t offset_before = alloc->offset;
    VecSizeT *vec = &alloc->prev_allocations;
    alloc->offset -= vec->items[vec->count-1];

    printf("[iNFO] Pop used_bytes: %zu -> %zu\n", offset_before, alloc->offset);

    vec_size_t_pop(&alloc->prev_allocations);
}

void stack_alloc_free(StackAlloc* alloc) {
    free(alloc->start);
    free(alloc->prev_allocations.items);
}

typedef struct {
    int x;
    int y;
    const char* string;
}Point;

int main () {
    // tests
    #define SIZE 1000
    StackAlloc stack_alloc = stack_alloc_build(SIZE);

    Point* point = (Point*)stack_alloc_add(&stack_alloc, sizeof(Point));
    point->x = +100;
    point->y = -100;
    point->string = "hello world";

    printf("x: %d\n",point->x);
    printf("y: %d\n",point->y);
    printf("z: %s\n",point->string);
    printf("z: %p\n",point->string);

    stack_alloc_free(&stack_alloc);
}

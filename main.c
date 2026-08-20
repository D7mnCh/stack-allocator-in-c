/*
- code review time
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define DEFAULT_CAPACITY 256

typedef struct {
    size_t  *items;
    // needed for malloc
    size_t  capacity;// nums of max elements
    // counts needed if i ran out of cap
    size_t  count;
}VecSizeT;

typedef struct {
    char *start;
    size_t offset;
    size_t size;
    // can't use an array cuz dync one can grow, the other can't
    VecSizeT prev_allocations;
} StackAlloc;

void vec_size_t_push (VecSizeT* vec, size_t item) {
    // used later to inialize the reallocated vector
    size_t old_capacity= vec->capacity;

    if (vec->capacity <= vec->count) {
        size_t new_capacity = vec->capacity = vec->capacity == 0 ? DEFAULT_CAPACITY: vec->capacity * 2;
        // i think it's dengerous to mutate vec->items, if it gives NULL then my prev vec->items
        //location is gone, and i can't access data on it anymore
        size_t *new_vec = vec->items = realloc(vec->items, vec->capacity * sizeof(size_t));
        if (new_vec == NULL) {
            printf("[ERROR] realloc failed\n");
            return;
        }

        size_t *new_vec_init = memset(new_vec + old_capacity, 0, (new_capacity - old_capacity)*sizeof(size_t));
        if (new_vec_init == NULL) {
            printf("[ERROR] memset failed\n");
            return;
        }

        vec->items    = new_vec;
        vec->capacity = new_capacity;
    }

    vec->items[vec->count++] = item;
}

void vec_size_t_pop (VecSizeT* vec) {
    if (vec->count == 0) {
        printf("[WARNING] VecSizeT is empty\n");
        return;
    }

    vec->items[--vec->count] = 0u;
}


StackAlloc stack_alloc_build(size_t size) {
    char* chunk = calloc(1, size);
    if (chunk == NULL) {
        printf("[ERROR] malloc failed\n");
        return (StackAlloc){0};
    }

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

    char* tracker = alloc->start + offset_before;
    return tracker;
}

// NOTE in order to pop, need to store allocation sizes
void stack_alloc_pop(StackAlloc* alloc) {
    if (alloc->prev_allocations.count == 0) {
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

    for (int i = 0; i <10; i++){
        printf("%zu\n", stack_alloc.prev_allocations.items[i]);
    }

    stack_alloc_pop(&stack_alloc);

    stack_alloc_free(&stack_alloc);
}

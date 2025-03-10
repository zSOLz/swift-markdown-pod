#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cmark-gfm.h>
#include <cmark-gfm-extension_api.h>
#include <mutex.h>

CMARK_DEFINE_LOCK(arena)

static struct arena_chunk {
  size_t sz, used;
  uint8_t push_point;
  void *ptr;
  struct arena_chunk *prev;
} *A = NULL;

static struct arena_chunk *alloc_arena_chunk(size_t sz, struct arena_chunk *prev) {
  struct arena_chunk *c = (struct arena_chunk *)calloc(1, sizeof(*c));
  if (!c)
    abort();
  c->sz = sz;
  c->ptr = calloc(1, sz);
  if (!c->ptr)
    abort();
  c->prev = prev;
  return c;
}

void cmark_arena_push(void) {
  CMARK_INITIALIZE_AND_LOCK(arena);
  if (A) {
    A->push_point = 1;
    A = alloc_arena_chunk(10240, A);
  }
  CMARK_UNLOCK(arena);
}

int cmark_arena_pop(void) {
  int ret = 1;
  CMARK_INITIALIZE_AND_LOCK(arena);
  if (!A)
    ret = 0;
  else {
    while (A && !A->push_point) {
      free(A->ptr);
      struct arena_chunk *n = A->prev;
      free(A);
      A = n;
    }
    if (A)
      A->push_point = 0;
  }
  CMARK_UNLOCK(arena);
  return ret;
}

static void init_arena(void) {
  CMARK_INITIALIZE_AND_LOCK(arena);
  A = alloc_arena_chunk(4 * 1048576, NULL);
  CMARK_UNLOCK(arena);
}

void cmark_arena_reset(void) {
  CMARK_INITIALIZE_AND_LOCK(arena);
  while (A) {
    free(A->ptr);
    struct arena_chunk *n = A->prev;
    free(A);
    A = n;
  }
  CMARK_UNLOCK(arena);
}

static void *arena_calloc(size_t nmem, size_t size) {
  if (!A)
    init_arena();

  size_t sz = nmem * size + sizeof(size_t);

  // Round allocation sizes to largest integer size to
  // ensure returned memory is correctly aligned
  const size_t align = sizeof(size_t) - 1;
  sz = (sz + align) & ~align;

  CMARK_INITIALIZE_AND_LOCK(arena);

  void *ptr = NULL;

  if (sz > A->sz) {
    A->prev = alloc_arena_chunk(sz, A->prev);
    ptr = (uint8_t *) A->prev->ptr;
  } else {
    if (sz > A->sz - A->used) {
      A = alloc_arena_chunk(A->sz + A->sz / 2, A);
    }
    ptr = (uint8_t *) A->ptr + A->used;
    A->used += sz;
    *((size_t *) ptr) = sz - sizeof(size_t);
  }
  
  CMARK_UNLOCK(arena);

  return (uint8_t *) ptr + sizeof(size_t);
}

static void *arena_realloc(void *ptr, size_t size) {
  if (!A)
    init_arena();

  void *new_ptr = arena_calloc(1, size);
  if (ptr)
    memcpy(new_ptr, ptr, ((size_t *) ptr)[-1]);
  return new_ptr;
}

static void arena_free(void *ptr) {
  (void) ptr;
  /* no-op */
}

cmark_mem CMARK_ARENA_MEM_ALLOCATOR = {arena_calloc, arena_realloc, arena_free};

cmark_mem *cmark_get_arena_mem_allocator() {
  return &CMARK_ARENA_MEM_ALLOCATOR;
}

#include "pxl_memory.h"

struct Block {
    size_t size;
    bool free;
    Block* next;
};

struct Arena {
    char* memory;
    size_t size;
    size_t offset;
};

static Block* heap_head = nullptr;
static Block* heap_tail = nullptr;

constexpr size_t BLOCK_SIZE = sizeof(Block);
constexpr size_t CHUNK_SIZE = 1024 * 1024; 

Block* find_free_block(size_t size) {
    Block* current = heap_head;
    while(current) {
        if(current->free && current->size >= size) 
            return current;
        current = current->next;
    }
    return nullptr;
}

Block* request_block_from_os(size_t size) {    
    size_t total_size = ((size + BLOCK_SIZE + CHUNK_SIZE - 1) / CHUNK_SIZE) * CHUNK_SIZE;
    void* memory = VirtualAlloc(nullptr, total_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(!memory) return nullptr;

    Block* _block = (Block*)memory;
    _block->size = total_size - BLOCK_SIZE;
    _block->free = false;
    _block->next = nullptr;

    if(!heap_head) 
        heap_head = _block;
    if(heap_tail)
        heap_tail->next = _block;
        
    heap_tail = _block;

    return _block;
}

void split_block(Block* block, size_t size) {
    if(block->size >= size + BLOCK_SIZE + 8) {
        Block* new_block = (Block*)((char*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = true;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;

        if(heap_tail == block) 
            heap_tail = new_block;
    }
}

void* _pxl_malloc(size_t size) {
    if(size == 0) return nullptr;

    Block* block = find_free_block(size);
    if(block) {
        block->free = false;
        split_block(block, size);
    } else {
        block = request_block_from_os(size);
        if(!block) return nullptr;
    }

    return (void*)(block + 1);
}

void* _pxl_realloc(void* ptr, size_t new_size) {
    if(!ptr) return _pxl_malloc(new_size);
    if(new_size == 0) {
        _pxl_free(ptr);
        return nullptr;   
    }

    Block* block = ((Block*)ptr) - 1;
    if(block->size >= new_size) return ptr;

    void* new_ptr = _pxl_malloc(new_size);
    if(new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        _pxl_free(ptr);
    }

    return new_ptr;
}

void* _pxl_calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = _pxl_malloc(total);
    if(ptr) memset(ptr, 0, total);
    return ptr;
}

void coalesce() {
    Block* current = heap_head;
    while(current && current->next) {
        if(current->free && current->next->free) {
            current->size += BLOCK_SIZE + current->next->size;
            current->next = current->next->next;
            if(current->next == nullptr) heap_tail = current;
        } else {
            current = current->next;
        }
    }
}

void _pxl_free(void* ptr) {
    if(!ptr) return;

    Block* block = ((Block*)ptr) - 1;
    assert(!block->free && "called twice!!");

    block->free = true;
    coalesce();
}

Arena* _pxl_arena_create(size_t size) {
    void* memory = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(!memory) return nullptr;

    Arena* _self = (Arena*)_pxl_malloc(sizeof(Arena));
    _self->memory = (char*)memory;
    _self->size = size;
    _self->offset = 0;

    return _self;
}

void* _pxl_arena_alloc(Arena* self, size_t size, size_t alignment = alignof(std::max_align_t)) {
    uintptr_t base = (uintptr_t)(self->memory + self->offset);
    uintptr_t aligned = (base + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned - base;

    if(self->offset + padding + size > self->size) {
        return _pxl_malloc(size);
    }

    self->offset += padding + size;
    return (void*) aligned;
}

void _pxl_arena_reset(Arena* self) {
    self->offset = 0;
}

void _pxl_arena_destroy(Arena* self) {
    VirtualFree(self->memory, 0, MEM_RELEASE);
    _pxl_free(self);
}
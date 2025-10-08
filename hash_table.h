#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>

#define HASH_TABLE_INITIAL_CAPACITY 1
#define HASH_TABLE_MAX_LOAD 1.0f

#define HASH_TABLE_TOMBSTONE ((entry_t*) 1)

typedef struct {
    const void* key;
    size_t key_length;
    const void* value;
} entry_t;

typedef struct {
    entry_t* entries;
    size_t size;
    size_t capacity;
} hash_table_t;

uint32_t hash(const void* key, size_t key_length);
void hash_table_init(hash_table_t* hash_table);
void hash_table_free(hash_table_t* hash_table);
void hash_table_set(hash_table_t* hash_table, const void* key, size_t key_length, const void* value);
const void* hash_table_get(hash_table_t* hash_table, const void* key, size_t key_length);

#endif

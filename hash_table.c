#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

uint32_t hash(const void* key, size_t key_length) {
    uint32_t hash = 2166136261u;
    const unsigned char* key_bytes = key;
    for (size_t i = 0; i < key_length; ++i) {
        hash ^= key_bytes[i];
        hash *= 16777619;
    }
    return hash;
}

void hash_table_init(hash_table_t* hash_table) {
    hash_table->entries = calloc(HASH_TABLE_INITIAL_CAPACITY, sizeof(*hash_table->entries));
    hash_table->size = 0;
    hash_table->capacity = HASH_TABLE_INITIAL_CAPACITY;
}

void hash_table_free(hash_table_t* hash_table) {
    free(hash_table->entries);
    hash_table->entries = NULL;
    hash_table->size = 0;
    hash_table->capacity = 0;
}

static inline entry_t* hash_table_find(entry_t* entries, size_t capacity, const void* key, size_t key_length) {
    entry_t* tombstone = NULL;
    for (uint32_t index = hash(key, key_length) % capacity; ; index = (index + 1) % capacity) {
        assert(index < capacity);
        entry_t* entry = &entries[index];
        if (entry->key == NULL) {
            if (entry->value != HASH_TABLE_TOMBSTONE) {
                return tombstone != NULL ? tombstone : entry;
            }
            if (tombstone == NULL) {
                tombstone = entry;
            }
            continue;
        }
        if (entry->key_length == key_length && memcmp(entry->key, key, key_length) == 0) {
            return entry;
        }
    }
}

static inline void hash_table_resize(hash_table_t* hash_table, size_t new_capacity) {
    entry_t* new_entries = calloc(new_capacity, sizeof(*new_entries));
    hash_table->size = 0;
    for (size_t i = 0; i < hash_table->capacity; ++i) {
        entry_t* old_entry = &hash_table->entries[i];
        if (old_entry->key == NULL) {
            continue;
        }
        entry_t* new_entry = hash_table_find(new_entries, new_capacity, old_entry->key, old_entry->key_length);
        *new_entry = *old_entry;
        ++hash_table->size;
    }
    free(hash_table->entries);
    hash_table->entries = new_entries;
    hash_table->capacity = new_capacity;
}

void hash_table_set(hash_table_t* hash_table, const void* key, size_t key_length, const void* value) {
    if (hash_table->size + 1.0f > hash_table->capacity * HASH_TABLE_MAX_LOAD) {
        hash_table_resize(hash_table, hash_table->capacity * 2);
    }
    entry_t* entry = hash_table_find(hash_table->entries, hash_table->capacity, key, key_length);
    if (entry->key == NULL && entry->value != HASH_TABLE_TOMBSTONE) {
        ++hash_table->size;
    }
    entry->key = key;
    entry->key_length = key_length;
    entry->value = value;
}

const void* hash_table_get(hash_table_t* hash_table, const void* key, size_t key_length) {
    if (hash_table->size == 0) {
        return NULL;
    }
    entry_t* entry = hash_table_find(hash_table->entries, hash_table->capacity, key, key_length);
    if (entry->key == NULL) {
        return NULL;
    }
    return entry->value;
}

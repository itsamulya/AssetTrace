#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "item.h"
#include "hash_index.h"
#include "storage.h"

/*
    Register a newly found item:
    - Generates new item ID (101 + *count)
    - Persists item to disk via appendItem
    - Updates in-memory items array
    - Updates hash table indexes (brand, color, name, category)
    - Returns 1 on success, 0 on failure.
*/
int registerItem(
    StoredItem items[],
    int *count,
    HashTable *hashTable,
    StoredItem item
);

#endif /* REGISTRATION_H */

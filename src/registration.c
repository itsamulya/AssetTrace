#include <stdio.h>
#include <string.h>

#include "registration.h"

int registerItem(
    StoredItem items[],
    int *count,
    HashTable *hashTable,
    StoredItem item
) {
    if (items == NULL || count == NULL || hashTable == NULL) {
        return 0;
    }

    if (*count >= MAX_ITEMS) {
        printf("\nDatabase is full.\n");
        return 0;
    }

    /*
        Generate ID starting from 101.
    */
    item.id = 101 + *count;
    strcpy(item.status, "Found");

    /*
        Persist to disk.
    */
    if (appendItem(item)) {
        printf("\nItem saved permanently.\n");
    } else {
        printf("\nWarning: Item could not be saved.\n");
        return 0;
    }

    /*
        Store in memory.
    */
    items[*count] = item;

    /*
        Update hash index.
    */
    insertIntoHashTable(hashTable, item.brand, item.id);
    insertIntoHashTable(hashTable, item.color, item.id);
    insertIntoHashTable(hashTable, item.name, item.id);
    insertIntoHashTable(hashTable, item.category, item.id);

    (*count)++;

    printf("Generated Item ID: %d\n", item.id);

    return 1;
}

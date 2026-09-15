#ifndef STORAGE_H
#define STORAGE_H

#include "item.h"

#define DATA_FILE "items.dat"

/*
    Load all items from file.

    Returns number of items loaded.
*/
int loadItems(
    StoredItem items[],
    int maxItems
);

/*
    Save all items to file.
*/
int saveItems(
    StoredItem items[],
    int count
);

/*
    Append a newly registered item.
*/
int appendItem(
    StoredItem item
);

#endif /* STORAGE_H */
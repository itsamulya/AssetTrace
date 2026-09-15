#include "item.h"

int verifyItemStructSize(void) {
    return (sizeof(StoredItem) == STORED_ITEM_SIZE);
}

StoredItem* findItemById(StoredItem items[], int count, int id) {
    if (items == NULL || count <= 0) {
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        if (items[i].id == id) {
            return &items[i];
        }
    }

    return NULL;
}

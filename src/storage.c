#include <stdio.h>
#include "storage.h"

/*
    Load items from persistent storage.
*/
int loadItems(
    StoredItem items[],
    int maxItems
) {
    if (!verifyItemStructSize()) {
        printf("Error: StoredItem struct size mismatch (%zu != %d). Binary I/O aborted.\n",
               sizeof(StoredItem), STORED_ITEM_SIZE);
        return 0;
    }

    FILE *file = fopen(DATA_FILE, "rb");

    /*
        File does not exist yet.
        This is normal on the first run.
    */
    if (file == NULL) {
        return 0;
    }

    int count = 0;

    while (
        count < maxItems
        && fread(&items[count], sizeof(StoredItem), 1, file) == 1
    ) {
        count++;
    }

    fclose(file);

    return count;
}

/*
    Save entire database.
*/
int saveItems(
    StoredItem items[],
    int count
) {
    if (!verifyItemStructSize()) {
        printf("Error: StoredItem struct size mismatch (%zu != %d). Binary I/O aborted.\n",
               sizeof(StoredItem), STORED_ITEM_SIZE);
        return 0;
    }

    FILE *file = fopen(DATA_FILE, "wb");

    if (file == NULL) {
        printf("Error: Could not open data file.\n");
        return 0;
    }

    size_t written = fwrite(
        items,
        sizeof(StoredItem),
        count,
        file
    );

    fclose(file);

    if (written != (size_t)count) {
        printf("Error: Could not save all items.\n");
        return 0;
    }

    return 1;
}

/*
    Append one item to the database.
*/
int appendItem(
    StoredItem item
) {
    if (!verifyItemStructSize()) {
        printf("Error: StoredItem struct size mismatch (%zu != %d). Binary I/O aborted.\n",
               sizeof(StoredItem), STORED_ITEM_SIZE);
        return 0;
    }

    FILE *file = fopen(DATA_FILE, "ab");

    if (file == NULL) {
        printf("Error: Could not open data file.\n");
        return 0;
    }

    size_t written = fwrite(
        &item,
        sizeof(StoredItem),
        1,
        file
    );

    fclose(file);

    if (written != 1) {
        return 0;
    }

    return 1;
}
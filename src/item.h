#ifndef ITEM_H
#define ITEM_H

#include <stddef.h>

#define MAX_ITEMS 100
#define ITEM_ID_START 101
#define STORED_ITEM_SIZE 676

typedef struct {
    int id;
    char category[50];
    char name[100];
    char brand[50];
    char color[30];
    char description[300];
    char location[100];
    char time[20];
    char status[20];
} StoredItem;

/* Compile-time verification that StoredItem binary size is exactly 676 bytes */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(StoredItem) == STORED_ITEM_SIZE, "StoredItem struct size must be exactly 676 bytes for binary compatibility");
#endif

/* Runtime verification that StoredItem binary size is exactly 676 bytes */
int verifyItemStructSize(void);

/* Find an item by its ID in an array of StoredItems */
StoredItem* findItemById(StoredItem items[], int count, int id);

#endif /* ITEM_H */

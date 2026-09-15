#include <stdio.h>

#include "item.h"
#include "storage.h"
#include "hash_index.h"
#include "graph.h"
#include "matcher.h"
#include "cli.h"

int main(void) {
    /*
        Database of stored items in memory.
    */
    StoredItem items[MAX_ITEMS];

    /*
        Load persistent data from binary file.
    */
    int count = loadItems(items, MAX_ITEMS);

    printf("\nLoaded %d found items from storage.\n", count);

    /*
        Initialize and build hash table indexes.
    */
    HashTable hashTable;
    buildIndexes(&hashTable, items, count);

    /*
        Initialize and build campus location graph.
    */
    CampusGraph campus;
    buildCampusGraph(&campus);

    /*
        Main CLI interaction loop.
    */
    int choice = 0;

    while (1) {
        displayMainMenu();

        if (!readInteger("Enter choice: ", &choice)) {
            printf("\nInvalid choice.\n");
            continue;
        }

        if (choice == 1) {
            handleRegisterFoundItem(items, &count, &hashTable);
        } else if (choice == 2) {
            handleSearchLostItem(items, count, &hashTable, &campus);
        } else if (choice == 3) {
            printf("\nItems currently stored: %d\n", count);
        } else if (choice == 4) {
            printf("\nAssetTrace closed.\n");
            break;
        } else {
            printf("\nInvalid choice.\n");
        }
    }

    /*
        Clean up dynamically allocated hash nodes before exiting.
    */
    freeHashTable(&hashTable);

    return 0;
}
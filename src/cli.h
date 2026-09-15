#ifndef CLI_H
#define CLI_H

#include <stddef.h>
#include "item.h"
#include "hash_index.h"
#include "graph.h"
#include "ranking.h"

/*
    Strict integer parsing from string buffer.
    Accepts only complete valid integers with optional surrounding whitespace.
    Rejects partially numeric input (e.g., "2abc", "1.5", "4xyz", "--1").
    Returns 1 on success, 0 on failure.
*/
int parseIntegerStrict(
    const char *buffer,
    int *value
);

/*
    Read an integer safely from stdin with strict validation.
    Prevents infinite loops when non-numeric or partial input is provided.
    Returns 1 on successful parsing, 0 on failure/invalid input.
*/
int readIntegerSafe(
    const char *prompt,
    int *value
);

/*
    Read a string safely from stdin using fgets.
    Strips trailing \r and \n.
    If user input exceeds buffer capacity, clears remaining characters from stdin.
*/
void readTextSafe(
    const char *prompt,
    char *buffer,
    size_t size
);

/*
    Backward-compatible aliases for existing callers.
*/
int readInteger(
    const char *prompt,
    int *value
);

void readText(
    const char *prompt,
    char text[],
    int size
);

/*
    Display the main application menu.
*/
void displayMainMenu(void);

/*
    Handle found item registration user interaction flow.
*/
void handleRegisterFoundItem(
    StoredItem items[],
    int *count,
    HashTable *hashTable
);

/*
    Handle lost item search and ranking user interaction flow.
*/
void handleSearchLostItem(
    StoredItem items[],
    int count,
    HashTable *hashTable,
    CampusGraph *campus
);

#endif /* CLI_H */

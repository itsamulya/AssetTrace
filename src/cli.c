#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "registration.h"
#include "matcher.h"

/*
    Read a string safely from stdin.
    - Strips trailing \r and \n.
    - If input was longer than buffer, flushes the remaining characters from stdin.
*/
void readTextSafe(
    const char *prompt,
    char *buffer,
    size_t size
) {
    if (buffer == NULL || size == 0) {
        return;
    }

    if (prompt != NULL) {
        printf("%s", prompt);
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    /* Check if newline exists in buffer */
    char *newlinePos = strpbrk(buffer, "\r\n");
    if (newlinePos != NULL) {
        /* Strip newline characters */
        *newlinePos = '\0';
    } else {
        /* Buffer was filled without newline: discard remaining characters up to newline/EOF */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard */
        }
    }
}

/*
    Strict integer parsing from string buffer.
    Accepts only complete valid integers with optional surrounding whitespace.
    Rejects partially numeric input (e.g. "2abc", "1.5", "4xyz", "--1").
*/
int parseIntegerStrict(
    const char *buffer,
    int *value
) {
    if (buffer == NULL || value == NULL) {
        return 0;
    }

    char extra;
    /* Format " %d %c" matches leading spaces, parses the int, and tries to match a trailing non-space char */
    if (sscanf(buffer, " %d %c", value, &extra) != 1) {
        return 0;
    }

    return 1;
}

/*
    Read an integer safely with strict validation.
    Reads full line to avoid leaving trailing invalid characters in stdin.
*/
int readIntegerSafe(
    const char *prompt,
    int *value
) {
    if (value == NULL) {
        return 0;
    }

    char buffer[64];
    readTextSafe(prompt, buffer, sizeof(buffer));

    if (buffer[0] == '\0') {
        return 0;
    }

    return parseIntegerStrict(buffer, value);
}

/*
    Backward-compatible wrappers.
*/
void readText(
    const char *prompt,
    char text[],
    int size
) {
    if (size > 0) {
        readTextSafe(prompt, text, (size_t)size);
    }
}

int readInteger(
    const char *prompt,
    int *value
) {
    return readIntegerSafe(prompt, value);
}

/*
    Display the main menu.
*/
void displayMainMenu(void) {
    printf("\n\n====================================\n");
    printf("             ASSETTRACE\n");
    printf("====================================\n");
    printf("1. Register Found Item\n");
    printf("2. Report Lost Item\n");
    printf("3. Show Stored Item Count\n");
    printf("4. Exit\n");
    printf("====================================\n");
}

/*
    Handle found item registration through CLI.
*/
void handleRegisterFoundItem(
    StoredItem items[],
    int *count,
    HashTable *hashTable
) {
    if (*count >= MAX_ITEMS) {
        printf("\nDatabase is full.\n");
        return;
    }

    StoredItem item;
    memset(&item, 0, sizeof(StoredItem));

    printf("\n====================================\n");
    printf("       REGISTER FOUND ITEM\n");
    printf("====================================\n");

    readTextSafe("Category: ", item.category, sizeof(item.category));
    readTextSafe("Name: ", item.name, sizeof(item.name));
    readTextSafe("Brand: ", item.brand, sizeof(item.brand));
    readTextSafe("Color: ", item.color, sizeof(item.color));
    readTextSafe("Location: ", item.location, sizeof(item.location));
    readTextSafe("Description: ", item.description, sizeof(item.description));
    readTextSafe("Time found: ", item.time, sizeof(item.time));

    registerItem(items, count, hashTable, item);
}

/*
    Handle search lost item flow through CLI.
*/
void handleSearchLostItem(
    StoredItem items[],
    int count,
    HashTable *hashTable,
    CampusGraph *campus
) {
    if (count == 0) {
        printf("\nNo found items are currently registered.\n");
        return;
    }

    StoredItem lostItem;
    memset(&lostItem, 0, sizeof(StoredItem));
    lostItem.id = 1;
    strcpy(lostItem.status, "Lost");

    printf("\n====================================\n");
    printf("          REPORT LOST ITEM\n");
    printf("====================================\n");

    readTextSafe("Category: ", lostItem.category, sizeof(lostItem.category));
    readTextSafe("Name: ", lostItem.name, sizeof(lostItem.name));
    readTextSafe("Brand: ", lostItem.brand, sizeof(lostItem.brand));
    readTextSafe("Color: ", lostItem.color, sizeof(lostItem.color));
    readTextSafe("Location: ", lostItem.location, sizeof(lostItem.location));
    readTextSafe("Description: ", lostItem.description, sizeof(lostItem.description));
    readTextSafe("Time lost (HH:MM): ", lostItem.time, sizeof(lostItem.time));

    /*
        Hash-based candidate retrieval.
    */
    int candidateIds[MAX_CANDIDATES];
    int candidateCount = retrieveCandidates(hashTable, lostItem, candidateIds, MAX_CANDIDATES);

    printf("\n====================================\n");
    printf("       CANDIDATE RETRIEVAL\n");
    printf("====================================\n");
    printf("Unique Candidates Found: %d\n", candidateCount);

    if (candidateCount == 0) {
        printf("\nNo potential matches found.\n");
        return;
    }

    /*
        Create max heap and score candidates.
    */
    MaxHeap heap;
    initializeHeap(&heap);

    for (int i = 0; i < candidateCount; i++) {
        StoredItem *candidate = findItemById(items, count, candidateIds[i]);
        if (candidate == NULL) {
            continue;
        }

        int finalScore = calculateTotalMatchScore(lostItem, *candidate, campus);
        insertHeap(&heap, candidate->id, finalScore);
    }

    /*
        Display ranked results.
    */
    printf("\n====================================\n");
    printf("          MATCH RESULTS\n");
    printf("====================================\n");

    int rank = 1;

    while (!isHeapEmpty(&heap)) {
        MatchResult result = extractMax(&heap);

        StoredItem *matchedItem = findItemById(items, count, result.itemId);
        if (matchedItem == NULL) {
            continue;
        }

        printf("\nRank #%d\n", rank);
        printf("Item ID     : %d\n", result.itemId);
        printf("Name        : %s\n", matchedItem->name);
        printf("Brand       : %s\n", matchedItem->brand);
        printf("Color       : %s\n", matchedItem->color);
        printf("Location    : %s\n", matchedItem->location);
        printf("Match Score : %d%%\n", result.score);

        rank++;
    }
}

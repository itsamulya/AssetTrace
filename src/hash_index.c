#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_index.h"

void initializeHashTable(
    HashTable *hashTable
) {
    if (hashTable == NULL) {
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable->table[i] = NULL;
    }
}

unsigned int hashFunction(
    const char *key
) {
    if (key == NULL) {
        return 0;
    }

    unsigned long hash = 5381;
    int c;

    while ((c = (unsigned char)*key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return (unsigned int)(hash % TABLE_SIZE);
}

void insertIntoHashTable(
    HashTable *hashTable,
    const char *key,
    int itemId
) {
    if (hashTable == NULL || key == NULL || key[0] == '\0') {
        return;
    }

    unsigned int index = hashFunction(key);

    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    strncpy(newNode->key, key, sizeof(newNode->key) - 1);
    newNode->key[sizeof(newNode->key) - 1] = '\0';
    newNode->itemId = itemId;
    newNode->next = hashTable->table[index];
    hashTable->table[index] = newNode;
}

int searchHashTable(
    HashTable *hashTable,
    const char *key,
    int candidateIds[]
) {
    if (hashTable == NULL || key == NULL || candidateIds == NULL) {
        return 0;
    }

    unsigned int index = hashFunction(key);
    HashNode *current = hashTable->table[index];
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (count < MAX_CANDIDATES) {
                candidateIds[count] = current->itemId;
                count++;
            }
        }
        current = current->next;
    }

    return count;
}

void freeHashTable(
    HashTable *hashTable
) {
    if (hashTable == NULL) {
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *current = hashTable->table[i];
        while (current != NULL) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
        hashTable->table[i] = NULL;
    }
}
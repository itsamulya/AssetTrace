#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#define TABLE_SIZE 101
#define MAX_CANDIDATES 100

typedef struct HashNode {
    char key[100];
    int itemId;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *table[TABLE_SIZE];
} HashTable;

void initializeHashTable(HashTable *hashTable);

unsigned int hashFunction(const char *key);

void insertIntoHashTable(
    HashTable *hashTable,
    const char *key,
    int itemId
);

int searchHashTable(
    HashTable *hashTable,
    const char *key,
    int candidateIds[]
);

void freeHashTable(HashTable *hashTable);

#endif /* HASH_INDEX_H */
#ifndef MATCHER_H
#define MATCHER_H

#include "item.h"
#include "hash_index.h"
#include "graph.h"
#include "ranking.h"
#include "text_match.h"

/*
    Build all hash indexes from loaded items.
*/
void buildIndexes(
    HashTable *hashTable,
    StoredItem items[],
    int count
);

/*
    Calculate attribute similarity score (0 to 95).
    Category (25) + Name (25) + Brand (20) + Color (15) + Location (10)
*/
int calculateAttributeScore(
    StoredItem lost,
    StoredItem found
);

/*
    Calculate location similarity score (0, 5, or 10) using campus graph Dijkstra.
*/
int calculateLocationScore(
    CampusGraph *campus,
    const char *lostLocation,
    const char *foundLocation
);

/*
    Calculate final combined match score (0 to 100).
*/
int calculateTotalMatchScore(
    StoredItem lost,
    StoredItem found,
    CampusGraph *campus
);

/*
    Retrieve deduplicated candidate item IDs matching brand, color, name, or category.
*/
int retrieveCandidates(
    HashTable *hashTable,
    StoredItem lostItem,
    int candidateIds[],
    int maxCandidates
);

/*
    Score candidates and insert them into the MaxHeap for ranking.
*/
int rankCandidates(
    StoredItem items[],
    int count,
    HashTable *hashTable,
    CampusGraph *campus,
    StoredItem lostItem,
    MaxHeap *heap
);

#endif /* MATCHER_H */

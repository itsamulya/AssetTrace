#include <stdio.h>
#include <string.h>

#include "matcher.h"

/*
    Build all hash indexes
    from the loaded database.
*/
void buildIndexes(
    HashTable *hashTable,
    StoredItem items[],
    int count
) {
    if (hashTable == NULL || items == NULL) {
        return;
    }

    initializeHashTable(hashTable);

    for (int i = 0; i < count; i++) {
        insertIntoHashTable(hashTable, items[i].brand, items[i].id);
        insertIntoHashTable(hashTable, items[i].color, items[i].id);
        insertIntoHashTable(hashTable, items[i].name, items[i].id);
        insertIntoHashTable(hashTable, items[i].category, items[i].id);
    }
}

/*
    Check whether candidate ID already exists in the candidate list.
*/
static int candidateExists(
    const int candidates[],
    int count,
    int itemId
) {
    for (int i = 0; i < count; i++) {
        if (candidates[i] == itemId) {
            return 1;
        }
    }
    return 0;
}

/*
    Retrieve candidates from hash table for a specific key.
*/
static int addCandidatesForKey(
    HashTable *hashTable,
    const char *key,
    int candidates[],
    int candidateCount,
    int maxCandidates
) {
    if (key == NULL || key[0] == '\0') {
        return candidateCount;
    }

    int temporary[MAX_CANDIDATES];
    int foundCount = searchHashTable(hashTable, key, temporary);

    for (int i = 0; i < foundCount; i++) {
        if (!candidateExists(candidates, candidateCount, temporary[i])) {
            if (candidateCount < maxCandidates) {
                candidates[candidateCount] = temporary[i];
                candidateCount++;
            }
        }
    }

    return candidateCount;
}

/*
    Retrieve all candidates matching brand, color, name, or category.
*/
int retrieveCandidates(
    HashTable *hashTable,
    StoredItem lostItem,
    int candidateIds[],
    int maxCandidates
) {
    int count = 0;

    count = addCandidatesForKey(hashTable, lostItem.brand, candidateIds, count, maxCandidates);
    count = addCandidatesForKey(hashTable, lostItem.color, candidateIds, count, maxCandidates);
    count = addCandidatesForKey(hashTable, lostItem.name, candidateIds, count, maxCandidates);
    count = addCandidatesForKey(hashTable, lostItem.category, candidateIds, count, maxCandidates);

    return count;
}

/*
    Calculate attribute similarity.
*/
int calculateAttributeScore(
    StoredItem lost,
    StoredItem found
) {
    int score = 0;

    if (strcmp(lost.category, found.category) == 0) {
        score += 25;
    }

    if (strcmp(lost.name, found.name) == 0) {
        score += 25;
    }

    if (strcmp(lost.brand, found.brand) == 0) {
        score += 20;
    }

    if (strcmp(lost.color, found.color) == 0) {
        score += 15;
    }

    if (strcmp(lost.location, found.location) == 0) {
        score += 10;
    }

    return score;
}

/*
    Calculate location similarity score.
*/
int calculateLocationScore(
    CampusGraph *campus,
    const char *lostLocationName,
    const char *foundLocationName
) {
    if (campus == NULL || lostLocationName == NULL || foundLocationName == NULL) {
        return 0;
    }

    int lostLocation = findLocation(campus, lostLocationName);
    int foundLocation = findLocation(campus, foundLocationName);

    if (lostLocation != -1 && foundLocation != -1) {
        int distance = dijkstra(campus, lostLocation, foundLocation);

        if (distance == 0) {
            return 10;
        } else if (distance != INF) {
            return 5;
        }
    }

    return 0;
}

/*
    Calculate final combined match score.
*/
int calculateTotalMatchScore(
    StoredItem lost,
    StoredItem found,
    CampusGraph *campus
) {
    int attributeScore = calculateAttributeScore(lost, found);
    int textScore = calculateTextSimilarity(lost.description, found.description);
    int locationScore = calculateLocationScore(campus, lost.location, found.location);

    int baseScore = (attributeScore * 80 + textScore * 20) / 100;
    int finalScore = baseScore + locationScore;

    if (finalScore > 100) {
        finalScore = 100;
    }

    return finalScore;
}

/*
    Score candidates and insert into the MaxHeap for ranking.
*/
int rankCandidates(
    StoredItem items[],
    int count,
    HashTable *hashTable,
    CampusGraph *campus,
    StoredItem lostItem,
    MaxHeap *heap
) {
    if (heap == NULL) {
        return 0;
    }

    initializeHeap(heap);

    int candidateIds[MAX_CANDIDATES];
    int candidateCount = retrieveCandidates(hashTable, lostItem, candidateIds, MAX_CANDIDATES);

    for (int i = 0; i < candidateCount; i++) {
        StoredItem *candidate = findItemById(items, count, candidateIds[i]);
        if (candidate == NULL) {
            continue;
        }

        int finalScore = calculateTotalMatchScore(lostItem, *candidate, campus);
        insertHeap(heap, candidate->id, finalScore);
    }

    return candidateCount;
}

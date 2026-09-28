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
    Calculate location similarity score (0 to 10) using campus graph Dijkstra distance:
    distance 0 -> 10, 1-3 -> 8, 4-6 -> 6, 7-9 -> 4, 10+ -> 2, unreachable/unknown -> 0.
*/
int calculateLocationScore(
    CampusGraph *campus,
    const char *lostLocation,
    const char *foundLocation
);

/*
    Parse a time string (e.g. "14:00", "2:00 PM", "10:30") into minutes from midnight (0 to 1439).
    Returns -1 for invalid or unparseable time format.
*/
int parseTimeToMinutes(
    const char *timeStr
);

/*
    Calculate temporal plausibility score (0 to 5) between lost and found report times:
    - Found before lost time -> 0
    - Difference 0–30 minutes -> 5
    - Difference 31–60 minutes -> 4
    - Difference 61–120 minutes -> 3
    - Difference 121–240 minutes -> 2
    - Difference > 240 minutes -> 1
    - Invalid or missing time -> 0
*/
int calculateTimeScore(
    const char *lostTime,
    const char *foundTime
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

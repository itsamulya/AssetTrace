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
    Calculate location similarity score based on Dijkstra shortest path distance:
    - distance == 0: 10 points (same location)
    - distance 1..3: 8 points
    - distance 4..6: 6 points
    - distance 7..9: 4 points
    - distance >= 10 (reachable): 2 points
    - unreachable / unknown location: 0 points
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
        } else if (distance >= 1 && distance <= 3) {
            return 8;
        } else if (distance >= 4 && distance <= 6) {
            return 6;
        } else if (distance >= 7 && distance <= 9) {
            return 4;
        } else if (distance != INF) {
            return 2;
        }
    }

    return 0;
}

/*
    Parse a time string (e.g. "14:00", "2:00 PM", "10:30") into minutes from midnight (0 to 1439).
    Returns -1 for invalid or unparseable time format.
*/
int parseTimeToMinutes(
    const char *timeStr
) {
    if (timeStr == NULL || timeStr[0] == '\0') {
        return -1;
    }

    int hours = -1;
    int minutes = -1;
    char period[10] = {0};

    int parsed = sscanf(timeStr, " %d : %d %9s", &hours, &minutes, period);
    if (parsed < 2) {
        return -1;
    }

    if (minutes < 0 || minutes > 59) {
        return -1;
    }

    if (parsed == 3) {
        /* 12-hour format with AM/PM */
        if (hours < 1 || hours > 12) {
            return -1;
        }

        if (period[0] == 'a' || period[0] == 'A') {
            if ((period[1] == 'm' || period[1] == 'M') && period[2] == '\0') {
                if (hours == 12) {
                    hours = 0;
                }
            } else {
                return -1;
            }
        } else if (period[0] == 'p' || period[0] == 'P') {
            if ((period[1] == 'm' || period[1] == 'M') && period[2] == '\0') {
                if (hours != 12) {
                    hours += 12;
                }
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    } else {
        /* 24-hour format (e.g. "14:00", "09:30") */
        if (hours < 0 || hours > 23) {
            return -1;
        }
    }

    return hours * 60 + minutes;
}

/*
    Calculate temporal plausibility score (0 to 5 points) assuming same operational day:
    - Found before lost time -> 0 points
    - Difference 0–30 minutes -> 5 points
    - Difference 31–60 minutes -> 4 points
    - Difference 61–120 minutes -> 3 points
    - Difference 121–240 minutes -> 2 points
    - Difference > 240 minutes -> 1 point
    - Invalid or missing time -> 0 points
*/
int calculateTimeScore(
    const char *lostTime,
    const char *foundTime
) {
    if (lostTime == NULL || foundTime == NULL) {
        return 0;
    }

    int lostMinutes = parseTimeToMinutes(lostTime);
    int foundMinutes = parseTimeToMinutes(foundTime);

    if (lostMinutes < 0 || foundMinutes < 0) {
        return 0;
    }

    int diffMinutes = foundMinutes - lostMinutes;

    if (diffMinutes < 0) {
        /* Found before lost time on the same operational day is impossible */
        return 0;
    } else if (diffMinutes <= 30) {
        return 5;
    } else if (diffMinutes <= 60) {
        return 4;
    } else if (diffMinutes <= 120) {
        return 3;
    } else if (diffMinutes <= 240) {
        return 2;
    } else {
        return 1;
    }
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
    int timeScore = calculateTimeScore(lost.time, found.time);

    int baseScore = (attributeScore * 80 + textScore * 20) / 100;
    int finalScore = baseScore + locationScore + timeScore;

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

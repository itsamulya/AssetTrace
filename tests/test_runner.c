#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "item.h"
#include "storage.h"
#include "hash_index.h"
#include "graph.h"
#include "ranking.h"
#include "text_match.h"
#include "matcher.h"
#include "registration.h"
#include "cli.h"

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define TEST_ASSERT(cond, msg) do { \
    if (cond) { \
        printf("  [PASS] %s\n", msg); \
        g_testsPassed++; \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
        g_testsFailed++; \
    } \
} while(0)

/*
    Test 1: Struct Size & Binary Compatibility Verification
*/
void test_struct_size(void) {
    printf("\n--- Running Test 1: StoredItem Binary Size & Layout ---\n");
    TEST_ASSERT(sizeof(StoredItem) == 676, "sizeof(StoredItem) is exactly 676 bytes");
    TEST_ASSERT(verifyItemStructSize() == 1, "verifyItemStructSize() returns 1");
}

/*
    Test 2: Hash Table Insertion, Search, Collisions & Cleanup
*/
void test_hash_table(void) {
    printf("\n--- Running Test 2: Custom Hash Table (Chaining, Collisions & Cleanup) ---\n");
    HashTable ht;
    initializeHashTable(&ht);

    for (int i = 0; i < TABLE_SIZE; i++) {
        assert(ht.table[i] == NULL);
    }
    TEST_ASSERT(1, "Hash table initialized with all NULL buckets");

    /* Test insertions and separate chaining */
    insertIntoHashTable(&ht, "Apple", 101);
    insertIntoHashTable(&ht, "Apple", 102);
    insertIntoHashTable(&ht, "Sony", 103);
    insertIntoHashTable(&ht, "Dell", 104);

    int candidates[MAX_CANDIDATES];
    int count = searchHashTable(&ht, "Apple", candidates);
    TEST_ASSERT(count == 2, "Found 2 candidates for key 'Apple'");
    TEST_ASSERT((candidates[0] == 102 && candidates[1] == 101) || (candidates[0] == 101 && candidates[1] == 102),
                "Candidate IDs match inserted values");

    count = searchHashTable(&ht, "Sony", candidates);
    TEST_ASSERT(count == 1 && candidates[0] == 103, "Found candidate for key 'Sony'");

    count = searchHashTable(&ht, "NonExistent", candidates);
    TEST_ASSERT(count == 0, "No candidates found for non-existent key");

    /* Null and empty key safety */
    insertIntoHashTable(&ht, "", 105);
    insertIntoHashTable(&ht, NULL, 106);
    count = searchHashTable(&ht, "", candidates);
    TEST_ASSERT(count == 0, "Empty key insertion safely ignored");
    count = searchHashTable(&ht, NULL, candidates);
    TEST_ASSERT(count == 0, "NULL key search safely handled");

    /* Test memory cleanup */
    freeHashTable(&ht);
    for (int i = 0; i < TABLE_SIZE; i++) {
        assert(ht.table[i] == NULL);
    }
    TEST_ASSERT(1, "freeHashTable freed all nodes and reset table buckets to NULL");
}

/*
    Test 3: Fuzzy / Overlap Text Matching & Normalization
*/
void test_text_matching(void) {
    printf("\n--- Running Test 3: Custom Text Matching & Tokenization ---\n");
    int score1 = calculateTextSimilarity("Black Boat Earbuds", "black boat earbuds");
    TEST_ASSERT(score1 == 100, "Identical text case-insensitive similarity is 100%");

    int score2 = calculateTextSimilarity("Black Boat Earbuds with charging case!", "black boat earbuds");
    TEST_ASSERT(score2 == 100, "Shorter text subset match is 100%");

    int score3 = calculateTextSimilarity("Red Leather Wallet", "Blue Plastic Bottle");
    TEST_ASSERT(score3 == 0, "Completely disjoint text similarity is 0%");

    int score4 = calculateTextSimilarity("", "Non-empty text");
    TEST_ASSERT(score4 == 0, "Empty string similarity against non-empty is 0%");

    int score5 = calculateTextSimilarity("", "");
    TEST_ASSERT(score5 == 0, "Empty string similarity against empty string is 0%");

    int score6 = calculateTextSimilarity("Boat Earbuds", "Sony Headphones");
    TEST_ASSERT(score6 == 0, "Different brand/item similarity is 0%");
}

/*
    Test 4: Campus Graph Construction & Dijkstra Shortest Path
*/
void test_graph_and_dijkstra(void) {
    printf("\n--- Running Test 4: Campus Graph & Dijkstra Shortest Path ---\n");
    CampusGraph campus;
    buildCampusGraph(&campus);

    TEST_ASSERT(campus.locationCount == 5, "Campus graph contains 5 standard locations");

    int library = findLocation(&campus, "Library");
    int blockA = findLocation(&campus, "Block A");
    int blockB = findLocation(&campus, "Block B");
    int canteen = findLocation(&campus, "Canteen");
    int mainGate = findLocation(&campus, "Main Gate");

    TEST_ASSERT(library == 0, "Library location index is 0");
    TEST_ASSERT(blockA == 1, "Block A location index is 1");
    TEST_ASSERT(blockB == 2, "Block B location index is 2");
    TEST_ASSERT(canteen == 3, "Canteen location index is 3");
    TEST_ASSERT(mainGate == 4, "Main Gate location index is 4");
    TEST_ASSERT(findLocation(&campus, "Unknown Place") == -1, "Unknown location name returns -1");
    TEST_ASSERT(findLocation(NULL, "Library") == -1, "NULL graph pointer returns -1");
    TEST_ASSERT(findLocation(&campus, NULL) == -1, "NULL location name returns -1");

    /* Shortest path tests */
    /* Direct: Library -> Block A = 3 */
    int distLibraryToBlockA = dijkstra(&campus, library, blockA);
    TEST_ASSERT(distLibraryToBlockA == 3, "Dijkstra shortest path Library -> Block A is 3");

    /* Path: Library -> Canteen (6) -> Main Gate (5) = 11 (shorter than Library->BlockA->BlockB->Gate = 13) */
    int distLibraryToGate = dijkstra(&campus, library, mainGate);
    TEST_ASSERT(distLibraryToGate == 11, "Dijkstra shortest path Library -> Main Gate is 11");

    /* Path: Block A -> Block B (2) -> Canteen (3) = 5 (shorter than BlockA->Library->Canteen = 9) */
    int distBlockAToCanteen = dijkstra(&campus, blockA, canteen);
    TEST_ASSERT(distBlockAToCanteen == 5, "Dijkstra shortest path Block A -> Canteen is 5");

    /* Distance to self */
    int distSelf = dijkstra(&campus, canteen, canteen);
    TEST_ASSERT(distSelf == 0, "Dijkstra distance to self is 0");

    /* Invalid vertex bounds */
    int distInvalid = dijkstra(&campus, -1, 2);
    TEST_ASSERT(distInvalid == INF, "Dijkstra with negative source returns INF");
    distInvalid = dijkstra(&campus, 0, 99);
    TEST_ASSERT(distInvalid == INF, "Dijkstra with out-of-bounds destination returns INF");

    /* Distance-based location scoring tests */
    int scoreSame = calculateLocationScore(&campus, "Library", "Library");
    TEST_ASSERT(scoreSame == 10, "Location score: same location (Library -> Library, dist 0) is 10");

    int scoreDist3 = calculateLocationScore(&campus, "Library", "Block A");
    TEST_ASSERT(scoreDist3 == 8, "Location score: Library -> Block A (dist 3, range 1-3) is 8");

    int scoreDist6 = calculateLocationScore(&campus, "Library", "Canteen");
    TEST_ASSERT(scoreDist6 == 6, "Location score: Library -> Canteen (dist 6, range 4-6) is 6");

    int scoreDist8 = calculateLocationScore(&campus, "Block B", "Main Gate");
    TEST_ASSERT(scoreDist8 == 4, "Location score: Block B -> Main Gate (dist 8, range 7-9) is 4");

    int scoreDist11 = calculateLocationScore(&campus, "Library", "Main Gate");
    TEST_ASSERT(scoreDist11 == 2, "Location score: Library -> Main Gate (dist 11, range 10+) is 2");

    int scoreUnknownDest = calculateLocationScore(&campus, "Library", "Unknown Place");
    TEST_ASSERT(scoreUnknownDest == 0, "Location score: unknown destination returns 0");

    int scoreUnknownSrc = calculateLocationScore(&campus, "Unknown Place", "Library");
    TEST_ASSERT(scoreUnknownSrc == 0, "Location score: unknown source returns 0");

    int scoreNullGraph = calculateLocationScore(NULL, "Library", "Block A");
    TEST_ASSERT(scoreNullGraph == 0, "Location score: NULL graph pointer returns 0");

    int scoreNullLoc = calculateLocationScore(&campus, NULL, "Block A");
    TEST_ASSERT(scoreNullLoc == 0, "Location score: NULL location name returns 0");
}

/*
    Test 5: Custom Max Heap Ranking & Priority Queue Order
*/
void test_max_heap(void) {
    printf("\n--- Running Test 5: Custom Max Heap Ranking ---\n");
    MaxHeap heap;
    initializeHeap(&heap);

    TEST_ASSERT(isHeapEmpty(&heap) == 1, "Heap is initially empty");

    insertHeap(&heap, 101, 50);
    insertHeap(&heap, 102, 95);
    insertHeap(&heap, 103, 75);
    insertHeap(&heap, 104, 85);
    insertHeap(&heap, 105, 100);

    TEST_ASSERT(isHeapEmpty(&heap) == 0, "Heap is not empty after insertions");
    TEST_ASSERT(heap.size == 5, "Heap size is 5");

    /* Verification of descending priority queue extraction order */
    MatchResult r1 = extractMax(&heap);
    TEST_ASSERT(r1.itemId == 105 && r1.score == 100, "1st extractMax has score 100 (item 105)");

    MatchResult r2 = extractMax(&heap);
    TEST_ASSERT(r2.itemId == 102 && r2.score == 95, "2nd extractMax has score 95 (item 102)");

    MatchResult r3 = extractMax(&heap);
    TEST_ASSERT(r3.itemId == 104 && r3.score == 85, "3rd extractMax has score 85 (item 104)");

    MatchResult r4 = extractMax(&heap);
    TEST_ASSERT(r4.itemId == 103 && r4.score == 75, "4th extractMax has score 75 (item 103)");

    MatchResult r5 = extractMax(&heap);
    TEST_ASSERT(r5.itemId == 101 && r5.score == 50, "5th extractMax has score 50 (item 101)");

    TEST_ASSERT(isHeapEmpty(&heap) == 1, "Heap is empty after extracting all elements");

    /* Extract from empty heap */
    MatchResult rEmpty = extractMax(&heap);
    TEST_ASSERT(rEmpty.itemId == -1 && rEmpty.score == -1, "Extract from empty heap returns sentinel {-1, -1}");
}

/*
    Test 6: Candidate Deduplication & Multi-Factor Scoring Engine
*/
void test_matching_engine(void) {
    printf("\n--- Running Test 6: Candidate Deduplication & Multi-Factor Scoring ---\n");
    CampusGraph campus;
    buildCampusGraph(&campus);

    HashTable ht;
    initializeHashTable(&ht);

    StoredItem items[2];
    memset(items, 0, sizeof(items));

    items[0].id = 101;
    strcpy(items[0].category, "Electronics");
    strcpy(items[0].name, "Earbuds");
    strcpy(items[0].brand, "Boat");
    strcpy(items[0].color, "Black");
    strcpy(items[0].location, "Library");
    strcpy(items[0].description, "Black Boat Earbuds in charging case");
    strcpy(items[0].time, "10:00 AM");
    strcpy(items[0].status, "Found");

    items[1].id = 102;
    strcpy(items[1].category, "Electronics");
    strcpy(items[1].name, "Headphones");
    strcpy(items[1].brand, "Boat");
    strcpy(items[1].color, "Blue");
    strcpy(items[1].location, "Canteen");
    strcpy(items[1].description, "Wireless bluetooth headphones");
    strcpy(items[1].time, "11:00 AM");
    strcpy(items[1].status, "Found");

    buildIndexes(&ht, items, 2);

    StoredItem query;
    memset(&query, 0, sizeof(query));
    query.id = 1;
    strcpy(query.category, "Electronics");
    strcpy(query.name, "Earbuds");
    strcpy(query.brand, "Boat");
    strcpy(query.color, "Black");
    strcpy(query.location, "Library");
    strcpy(query.description, "Black Boat Earbuds");
    strcpy(query.time, "10:00 AM");
    strcpy(query.status, "Lost");

    /* Candidate deduplication test */
    int candidateIds[MAX_CANDIDATES];
    int candCount = retrieveCandidates(&ht, query, candidateIds, MAX_CANDIDATES);
    TEST_ASSERT(candCount == 2, "Found 2 unique candidates matching query criteria");
    TEST_ASSERT((candidateIds[0] == 101 && candidateIds[1] == 102) || (candidateIds[0] == 102 && candidateIds[1] == 101),
                "Candidate list contains IDs 101 and 102 without duplicates");

    /* Attribute scores:
       Item 101: Category(25) + Name(25) + Brand(20) + Color(15) + Location(10) = 95
       Item 102: Category(25) + Brand(20) = 45 */
    int attrScore101 = calculateAttributeScore(query, items[0]);
    TEST_ASSERT(attrScore101 == 95, "Item 101 attribute score is 95");
    int attrScore102 = calculateAttributeScore(query, items[1]);
    TEST_ASSERT(attrScore102 == 45, "Item 102 attribute score is 45");

    /* Location scores */
    int locScore101 = calculateLocationScore(&campus, query.location, items[0].location);
    TEST_ASSERT(locScore101 == 10, "Item 101 same-location score is 10");
    int locScore102 = calculateLocationScore(&campus, query.location, items[1].location);
    TEST_ASSERT(locScore102 == 6, "Item 102 connected-location score (Library->Canteen, dist 6) is 6");

    /* Time scores */
    int timeScore101 = calculateTimeScore(query.time, items[0].time);
    TEST_ASSERT(timeScore101 == 5, "Item 101 same-time score (10:00 AM -> 10:00 AM, diff 0) is 5");
    int timeScore102 = calculateTimeScore(query.time, items[1].time);
    TEST_ASSERT(timeScore102 == 4, "Item 102 time score (10:00 AM -> 11:00 AM, diff 60m) is 4");

    /* Total score */
    int total101 = calculateTotalMatchScore(query, items[0], &campus);
    TEST_ASSERT(total101 == 100, "Item 101 total score is capped at 100%");
    int total102 = calculateTotalMatchScore(query, items[1], &campus);
    TEST_ASSERT(total102 == 46, "Item 102 total score is 46% ((45*80 + 0*20)/100 + 6 + 4)");

    /* Ranking test */
    MaxHeap heap;
    rankCandidates(items, 2, &ht, &campus, query, &heap);
    TEST_ASSERT(heap.size == 2, "Rank heap contains 2 scored candidates");
    MatchResult first = extractMax(&heap);
    TEST_ASSERT(first.itemId == 101 && first.score == 100, "Top ranked candidate is Item 101 (100%)");
    MatchResult second = extractMax(&heap);
    TEST_ASSERT(second.itemId == 102 && second.score == 46, "Second ranked candidate is Item 102 (46%)");

    freeHashTable(&ht);
}

/*
    Test 7: Persistence Loading, Item Lookup & In-Memory Storage
*/
void test_storage_and_item_lookup(void) {
    printf("\n--- Running Test 7: Persistence Loading & Item Lookup ---\n");
    StoredItem items[MAX_ITEMS];
    int count = loadItems(items, MAX_ITEMS);
    printf("  Loaded %d item(s) from existing items.dat\n", count);
    TEST_ASSERT(count >= 0, "loadItems executed cleanly without errors");

    if (count > 0) {
        StoredItem *found = findItemById(items, count, items[0].id);
        TEST_ASSERT(found != NULL && found->id == items[0].id, "findItemById located existing item");
    }

    StoredItem *notFound = findItemById(items, count, 999999);
    TEST_ASSERT(notFound == NULL, "findItemById returns NULL for invalid ID");
    StoredItem *nullArray = findItemById(NULL, 10, 101);
    TEST_ASSERT(nullArray == NULL, "findItemById returns NULL when array is NULL");
}

/*
    Test 8: Strict Integer Parsing & Error Rejection
*/
void test_strict_integer_parsing(void) {
    printf("\n--- Running Test 8: Strict Integer Parsing & Rejection ---\n");
    int val = 0;

    /* Rejections */
    TEST_ASSERT(parseIntegerStrict("2abc", &val) == 0, "Rejects '2abc'");
    TEST_ASSERT(parseIntegerStrict("1.5", &val) == 0, "Rejects '1.5'");
    TEST_ASSERT(parseIntegerStrict("4xyz", &val) == 0, "Rejects '4xyz'");
    TEST_ASSERT(parseIntegerStrict("--1", &val) == 0, "Rejects '--1'");
    TEST_ASSERT(parseIntegerStrict("abc", &val) == 0, "Rejects 'abc'");
    TEST_ASSERT(parseIntegerStrict("", &val) == 0, "Rejects empty string");
    TEST_ASSERT(parseIntegerStrict("   ", &val) == 0, "Rejects whitespace-only string");
    TEST_ASSERT(parseIntegerStrict("1 2", &val) == 0, "Rejects '1 2'");
    TEST_ASSERT(parseIntegerStrict(NULL, &val) == 0, "Rejects NULL input");

    /* Acceptances */
    TEST_ASSERT(parseIntegerStrict("3", &val) == 1 && val == 3, "Accepts '3'");
    TEST_ASSERT(parseIntegerStrict("  42  ", &val) == 1 && val == 42, "Accepts '  42  ' with surrounding spaces");
    TEST_ASSERT(parseIntegerStrict("-5", &val) == 1 && val == -5, "Accepts '-5'");
    TEST_ASSERT(parseIntegerStrict("0", &val) == 1 && val == 0, "Accepts '0'");
}

/*
    Test 9: Edge Cases (Capacity Limits, No Candidates, Unknown Locations)
*/
void test_edge_cases(void) {
    printf("\n--- Running Test 9: Edge Cases (Capacity, Unknowns, Empty Matches) ---\n");

    /* Test registration capacity limit */
    StoredItem items[MAX_ITEMS];
    int count = MAX_ITEMS;
    HashTable ht;
    initializeHashTable(&ht);

    StoredItem newItem;
    memset(&newItem, 0, sizeof(newItem));
    strcpy(newItem.name, "ExtraItem");

    int regResult = registerItem(items, &count, &ht, newItem);
    TEST_ASSERT(regResult == 0, "registerItem returns 0 when database capacity is full");

    /* Test matching with 0 registered items */
    StoredItem lostItem;
    memset(&lostItem, 0, sizeof(lostItem));
    strcpy(lostItem.brand, "UnknownBrand");

    int candidateIds[MAX_CANDIDATES];
    int candCount = retrieveCandidates(&ht, lostItem, candidateIds, MAX_CANDIDATES);
    TEST_ASSERT(candCount == 0, "retrieveCandidates returns 0 when no items match");

    /* Test location score with completely unknown locations */
    CampusGraph campus;
    buildCampusGraph(&campus);
    int locScore = calculateLocationScore(&campus, "Mars", "Jupiter");
    TEST_ASSERT(locScore == 0, "calculateLocationScore returns 0 for unknown locations");

    freeHashTable(&ht);
}

/*
    Test 10: Time Parsing & Temporal Plausibility Scoring
*/
void test_time_plausibility(void) {
    printf("\n--- Running Test 10: Time Parsing & Temporal Plausibility Scoring ---\n");

    /* 1. Time parsing unit tests */
    TEST_ASSERT(parseTimeToMinutes("14:00") == 840, "parseTimeToMinutes('14:00') is 840");
    TEST_ASSERT(parseTimeToMinutes("00:00") == 0, "parseTimeToMinutes('00:00') is 0");
    TEST_ASSERT(parseTimeToMinutes("23:59") == 1439, "parseTimeToMinutes('23:59') is 1439");
    TEST_ASSERT(parseTimeToMinutes("10:00 AM") == 600, "parseTimeToMinutes('10:00 AM') is 600");
    TEST_ASSERT(parseTimeToMinutes("12:00 PM") == 720, "parseTimeToMinutes('12:00 PM') is 720 (noon)");
    TEST_ASSERT(parseTimeToMinutes("12:00 AM") == 0, "parseTimeToMinutes('12:00 AM') is 0 (midnight)");
    TEST_ASSERT(parseTimeToMinutes("2:30 PM") == 870, "parseTimeToMinutes('2:30 PM') is 870");
    TEST_ASSERT(parseTimeToMinutes("24:00") == -1, "parseTimeToMinutes('24:00') rejects hour 24");
    TEST_ASSERT(parseTimeToMinutes("14:60") == -1, "parseTimeToMinutes('14:60') rejects minute 60");
    TEST_ASSERT(parseTimeToMinutes("") == -1, "parseTimeToMinutes('') returns -1");
    TEST_ASSERT(parseTimeToMinutes(NULL) == -1, "parseTimeToMinutes(NULL) returns -1");
    TEST_ASSERT(parseTimeToMinutes("invalid") == -1, "parseTimeToMinutes('invalid') returns -1");

    /* 2. Core time plausibility test cases from specification */
    TEST_ASSERT(calculateTimeScore("14:00", "14:20") == 5, "14:00 lost / 14:20 found (diff 20m, 0-30m) is 5");
    TEST_ASSERT(calculateTimeScore("14:00", "14:45") == 4, "14:00 lost / 14:45 found (diff 45m, 31-60m) is 4");
    TEST_ASSERT(calculateTimeScore("14:00", "15:30") == 3, "14:00 lost / 15:30 found (diff 90m, 61-120m) is 3");
    TEST_ASSERT(calculateTimeScore("14:00", "17:00") == 2, "14:00 lost / 17:00 found (diff 180m, 121-240m) is 2");
    TEST_ASSERT(calculateTimeScore("14:00", "19:00") == 1, "14:00 lost / 19:00 found (diff 300m, >240m) is 1");
    TEST_ASSERT(calculateTimeScore("14:00", "13:50") == 0, "14:00 lost / 13:50 found (found before lost) is 0");
    TEST_ASSERT(calculateTimeScore("14:00", "invalid") == 0, "14:00 lost / invalid found returns 0");
    TEST_ASSERT(calculateTimeScore("invalid", "14:00") == 0, "invalid lost / 14:00 found returns 0");
    TEST_ASSERT(calculateTimeScore(NULL, "14:00") == 0, "NULL lost time returns 0");
    TEST_ASSERT(calculateTimeScore("14:00", NULL) == 0, "NULL found time returns 0");

    /* 3. Exact boundary condition testing */
    TEST_ASSERT(calculateTimeScore("14:00", "14:00") == 5, "Boundary diff = 0 min is 5");
    TEST_ASSERT(calculateTimeScore("14:00", "14:30") == 5, "Boundary diff = 30 min is 5");
    TEST_ASSERT(calculateTimeScore("14:00", "14:31") == 4, "Boundary diff = 31 min is 4");
    TEST_ASSERT(calculateTimeScore("14:00", "15:00") == 4, "Boundary diff = 60 min is 4");
    TEST_ASSERT(calculateTimeScore("14:00", "15:01") == 3, "Boundary diff = 61 min is 3");
    TEST_ASSERT(calculateTimeScore("14:00", "16:00") == 3, "Boundary diff = 120 min is 3");
    TEST_ASSERT(calculateTimeScore("14:00", "16:01") == 2, "Boundary diff = 121 min is 2");
    TEST_ASSERT(calculateTimeScore("14:00", "18:00") == 2, "Boundary diff = 240 min is 2");
    TEST_ASSERT(calculateTimeScore("14:00", "18:01") == 1, "Boundary diff = 241 min is 1");
    TEST_ASSERT(calculateTimeScore("14:00", "13:59") == 0, "Boundary diff = -1 min (found before lost) is 0");
}

int main(void) {
    printf("========================================\n");
    printf("       AssetTrace Test Runner\n");
    printf("========================================\n");

    test_struct_size();
    test_hash_table();
    test_text_matching();
    test_graph_and_dijkstra();
    test_max_heap();
    test_matching_engine();
    test_storage_and_item_lookup();
    test_strict_integer_parsing();
    test_edge_cases();
    test_time_plausibility();

    printf("\n========================================\n");
    printf("Test Results: %d Passed, %d Failed\n", g_testsPassed, g_testsFailed);
    printf("========================================\n");

    return (g_testsFailed == 0) ? 0 : 1;
}

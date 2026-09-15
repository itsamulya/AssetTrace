#ifndef RANKING_H
#define RANKING_H

#define MAX_RESULTS 100

typedef struct {
    int itemId;
    int score;
} MatchResult;

typedef struct {
    MatchResult heap[MAX_RESULTS];
    int size;
} MaxHeap;

void initializeHeap(MaxHeap *heap);

void insertHeap(
    MaxHeap *heap,
    int itemId,
    int score
);

MatchResult extractMax(
    MaxHeap *heap
);

int isHeapEmpty(
    MaxHeap *heap
);

#endif
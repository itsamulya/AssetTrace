#include <stdio.h>

#include "ranking.h"


/*
    Swap two match results.
*/
void swap(
    MatchResult *a,
    MatchResult *b
) {

    MatchResult temp = *a;

    *a = *b;

    *b = temp;
}


/*
    Initialize the max heap.
*/
void initializeHeap(
    MaxHeap *heap
) {

    heap->size = 0;
}


/*
    Move an element upward
    until the max heap property
    is restored.
*/
void heapifyUp(
    MaxHeap *heap,
    int index
) {

    while (index > 0) {

        int parent =
            (index - 1) / 2;


        /*
            If parent already has
            a greater score, stop.
        */

        if (
            heap->heap[parent].score
            >=
            heap->heap[index].score
        ) {

            break;
        }


        swap(
            &heap->heap[parent],
            &heap->heap[index]
        );


        index = parent;
    }
}


/*
    Insert a match result into
    the max heap.
*/
void insertHeap(
    MaxHeap *heap,
    int itemId,
    int score
) {

    if (
        heap->size >= MAX_RESULTS
    ) {

        printf(
            "Heap is full.\n"
        );

        return;
    }


    int index =
        heap->size;


    heap->heap[index].itemId =
        itemId;

    heap->heap[index].score =
        score;


    heap->size++;


    heapifyUp(
        heap,
        index
    );
}


/*
    Move an element downward
    until the max heap property
    is restored.
*/
void heapifyDown(
    MaxHeap *heap,
    int index
) {

    while (1) {

        int left =
            2 * index + 1;

        int right =
            2 * index + 2;

        int largest =
            index;


        if (
            left < heap->size
            &&
            heap->heap[left].score
            >
            heap->heap[largest].score
        ) {

            largest = left;
        }


        if (
            right < heap->size
            &&
            heap->heap[right].score
            >
            heap->heap[largest].score
        ) {

            largest = right;
        }


        if (largest == index) {

            break;
        }


        swap(
            &heap->heap[index],
            &heap->heap[largest]
        );


        index = largest;
    }
}


/*
    Remove and return
    the highest-scoring result.
*/
MatchResult extractMax(
    MaxHeap *heap
) {

    MatchResult result = {
        -1,
        -1
    };


    if (
        heap->size == 0
    ) {

        return result;
    }


    result =
        heap->heap[0];


    heap->size--;


    if (
        heap->size > 0
    ) {

        heap->heap[0] =
            heap->heap[heap->size];


        heapifyDown(
            heap,
            0
        );
    }


    return result;
}


/*
    Check whether heap is empty.
*/
int isHeapEmpty(
    MaxHeap *heap
) {

    return heap->size == 0;
}
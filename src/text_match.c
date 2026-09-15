#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text_match.h"

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 50


/*
    Convert a string to lowercase.
*/
void convertToLower(
    char *text
) {

    for (
        int i = 0;
        text[i] != '\0';
        i++
    ) {

        text[i] =
            (char)tolower(
                (unsigned char)text[i]
            );
    }
}


/*
    Check whether a word is already present
    in an array of words.
*/
int wordExists(
    char words[][MAX_WORD_LENGTH],
    int count,
    const char *target
) {

    for (
        int i = 0;
        i < count;
        i++
    ) {

        if (
            strcmp(
                words[i],
                target
            ) == 0
        ) {

            return 1;
        }
    }

    return 0;
}


/*
    Split a sentence into individual words.

    Example:

    "Black Boat Earbuds"

    becomes:

    black
    boat
    earbuds
*/
int tokenize(
    const char *text,
    char words[][MAX_WORD_LENGTH]
) {

    char copy[500];

    strncpy(
        copy,
        text,
        sizeof(copy) - 1
    );

    copy[
        sizeof(copy) - 1
    ] = '\0';


    convertToLower(copy);


    int count = 0;


    char *token =
        strtok(
            copy,
            " ,.!?;:-_()[]{}"
        );


    while (
        token != NULL
        &&
        count < MAX_WORDS
    ) {

        /*
            Ignore extremely short words
            such as "a", "an", "of".
        */

        if (
            strlen(token) > 1
            &&
            !wordExists(
                words,
                count,
                token
            )
        ) {

            strncpy(
                words[count],
                token,
                MAX_WORD_LENGTH - 1
            );

            words[count][
                MAX_WORD_LENGTH - 1
            ] = '\0';

            count++;
        }


        token =
            strtok(
                NULL,
                " ,.!?;:-_()[]{}"
            );
    }


    return count;
}


/*
    Calculate fuzzy text similarity.

    Returns 0-100.
*/
int calculateTextSimilarity(
    const char *text1,
    const char *text2
) {

    char words1[
        MAX_WORDS
    ][
        MAX_WORD_LENGTH
    ];


    char words2[
        MAX_WORDS
    ][
        MAX_WORD_LENGTH
    ];


    int count1 =
        tokenize(
            text1,
            words1
        );


    int count2 =
        tokenize(
            text2,
            words2
        );


    if (
        count1 == 0
        ||
        count2 == 0
    ) {

        return 0;
    }


    /*
        Count common words.
    */

    int commonWords = 0;


    for (
        int i = 0;
        i < count1;
        i++
    ) {

        if (
            wordExists(
                words2,
                count2,
                words1[i]
            )
        ) {

            commonWords++;
        }
    }


    /*
        Use the smaller word count
        as the denominator.

        This prevents a long description
        from unfairly reducing similarity.
    */

    int smallerCount =
        count1 < count2
        ? count1
        : count2;


    int score =
        (
            commonWords * 100
        )
        /
        smallerCount;


    if (
        score > 100
    ) {

        score = 100;
    }


    return score;
}
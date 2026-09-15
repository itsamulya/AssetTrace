#ifndef TEXT_MATCH_H
#define TEXT_MATCH_H

/*
    Returns a similarity score from 0 to 100.

    The score is based on the percentage of
    words from the shorter description that
    also appear in the other description.
*/
int calculateTextSimilarity(
    const char *text1,
    const char *text2
);

#endif

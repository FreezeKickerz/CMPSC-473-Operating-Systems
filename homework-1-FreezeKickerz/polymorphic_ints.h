#include <stdio.h>
#include <stdlib.h>

// Union with a tag
typedef struct {
    enum { INT, CHAR4 } tag;
    union {
        int asInt;      // 32-bit integer
        unsigned char asChar[4]; // 4 8-bit numbers
    } data;
} TaggedUnion;

// Function to add the values of a list of TaggedUnion
int addTaggedUnions(TaggedUnion *unions, size_t count);

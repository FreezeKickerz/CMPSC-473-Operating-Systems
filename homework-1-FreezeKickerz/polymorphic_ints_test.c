#include "polymorphic_ints.h"
#include <assert.h>

#define mu_str_(text) #text
#define mu_str(text) mu_str_(text)
#define mu_assert(message, test) do { if (!(test)) return "See " __FILE__ " Line " mu_str(__LINE__) ": " message; } while (0)

// Test Case 1: Example case
char* testExampleCase() {
    TaggedUnion unions[3];
    unions[0].tag = INT;
    unions[0].data.asInt = 42;

    unions[1].tag = CHAR4;
    unions[1].data.asChar[0] = 10;
    unions[1].data.asChar[1] = 20;
    unions[1].data.asChar[2] = 30;
    unions[1].data.asChar[3] = 40;

    unions[2].tag = INT;
    unions[2].data.asInt = 100;

    int result = addTaggedUnions(unions, 3);
    mu_assert("Incorrect sum for sample test case, should be 242", result == 242);
    return NULL;
}

// Test Case 2: Empty array
char* testEmptyArray() {
    TaggedUnion unions[0];
    int result = addTaggedUnions(unions, 0);
    mu_assert("Incorrect sum for empty array case, should be 0", result == 0);
    return NULL;
}

int main() {
    char* (*testFunctions[])() = {
        testExampleCase,
        testEmptyArray
    };

    const int numTests = sizeof(testFunctions) / sizeof(testFunctions[0]);

    int noOfTestCasesPassed = 0;
    for (int i = 0; i < numTests; i++) {
        char* message = testFunctions[i]();
        if (message) {
            printf("FAILURE: %s\n", message);
        } else {
            noOfTestCasesPassed++;
        }
    }

    printf("Tagged Union Test Cases Passed: %d/%d\n", noOfTestCasesPassed, numTests);
    return 0;
}

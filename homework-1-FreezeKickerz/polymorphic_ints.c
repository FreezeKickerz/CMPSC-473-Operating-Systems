//Name: Tyler Korz
//PSU Email: tkk297@psu.edu
//PSU ID: 967407712

#include "polymorphic_ints.h"

// Function to add the values of a list of TaggedUnion
int addTaggedUnions(TaggedUnion *unions, size_t count) {
    //TODO: IMPLEMENT THIS
   int sum = 0;
    for (size_t i = 0; i < count; i++) {
        //Chooses between the two tags for each of the Tagged unions in the array
        switch (unions[i].tag) {
            case INT:
                // if int add
                sum += unions[i].data.asInt;
                break;
                
            case CHAR4:
                // if char sum
                sum += unions[i].data.asChar[0] + unions[i].data.asChar[1] + unions[i].data.asChar[2] + unions[i].data.asChar[3];
                break;
            //In case of empty array
            default:
                break;
        }
    }
    return sum;
}    

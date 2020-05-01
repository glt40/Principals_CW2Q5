
#ifndef C_STUFF_HASHTABLE_H
#define C_STUFF_HASHTABLE_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Accessible Functions
void ht_create();
void ht_add(char *string);
void ht_remove(char *string);
bool ht_search(char *string);
void ht_destroy();

#endif //C_STUFF_HASHTABLE_H

#include "hashtable.h"

// Initialise data structure
typedef struct {
   char **ht_record;
} ht_hashtable;

// Initialise global variables
ht_hashtable hashtable;
int table_size = 1024;
bool table_exists = false;

/*The following function finds the length of a string, given a pointer to the first char, returning an int
 */
int find_len(char *string) {
    int i=0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}

/* This hashing algorithm was created by Bob Jenkins in an article in "Dr. Dobbs" in September 1997
 * It is fast and due to its performance, few collisions are expected
 * The algorithm is called Jenkins' one-at-a-time hash and returns a 32-bit unsigned integer
 */
__uint32_t joaat_hash(char *key) {
    size_t key_len = sizeof(find_len(key));
    __uint32_t hash = 0;
    size_t i;

    for (i = 0; i < key_len; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

/* This function initialises a new table by allocating a fixed amount of memory and clearing it to NULL
 * The pointer to the table is stored in a global variable
 * The table has space for default 8192 strings, but it is recommended to store up to 75% of that
 * NOTE THAT THE HASHTABLE IS CASE SENSITIVE
 */
void ht_create() {
    if (table_exists) {
        // Don't create a new table if there's one already there
        printf("Table already exists. Aborting function.\n");
        return;
    }
    // Allocate the memory
    hashtable.ht_record = (char **)malloc(sizeof(char *) * table_size);
    // Set all records to null to allow for searching
    for (int i = 0; i < table_size; i++) {
        hashtable.ht_record[i] = NULL;
    }
    table_exists = true;
    printf("Table created.\n");
}

/* This function finds the next empty space in the hashtable starting from a given index
 * Returns an index, or returns the table size as an error if table is full, default 8192
 */
int find_empty(int start) {
    // collision handling: if the index calculated contains something, find the next empty one
    for (int i = 0; i < table_size; i++) {
        if (hashtable.ht_record[(start + i) % table_size] == NULL) {
            // This mod expression allows for wraparound in the array
            return (i + start) % table_size;
        }
    }
    // If no empty spaces are found return error
    return table_size;
}

/* This function adds a string to the hashtable by hashing the string,
 * then using the modulo division by table size to generate an index
 * Collision handling is done via Open addressing/ Linear probing
 */
void ht_add(char *string) {
    if (!table_exists) {
        printf("No table found. Aborting function.\n");
        return;
    }
    int str_len = find_len(string);
    int dest_index = (int)(joaat_hash(string) % table_size);
    // Find the next empty index with find_empty()
    dest_index = find_empty(dest_index);
    if (dest_index == table_size) {
        // If there are no free spaces left, the string can't be added
        printf("Error adding \"%s\". Table is full.\n", string);
        return;
    }
    // Adding string (array) to the table (array of arrays) at index i
    // Memory needs to be allocated for the string, including the NULL char
    hashtable.ht_record[dest_index] = malloc(sizeof(char)*(str_len+1));
    for (int j = 0; j <= str_len; j++) {
        // Copy each letter into the record
        hashtable.ht_record[dest_index][j] = string[j];
    }
    // printf( "\"%s\" successfully added to table at index %d.\n", string, dest_index);
}

/* This function takes a char pointer to a string and returns the index at which that string is found
 * Returns table size as an error if table is full or if string isn't found
 */
int ht_find_ind(char *string) {
    int str_len = find_len(string);
    int dest_index = (int)(joaat_hash(string) % table_size);
    int stop_index = find_empty(dest_index);
    int num_items;
    bool full_match = false;
    int i;
    // The string won't always be in the expected index, in which case you have to trawl up to the next empty index
    // if there are no empty indexes, trawl the whole table
    if (stop_index == table_size) {
        num_items = table_size;
    } else {
        if (stop_index > dest_index) {
            num_items = stop_index - dest_index;
        } else {
            // stop_index is before dest_index so check up to the end of the table and then from [0] to stop_index
            num_items = table_size - (dest_index - stop_index);
        }
    }
    for (i = 0; i < num_items; i++) {
        if (hashtable.ht_record[(dest_index + i) % table_size] == NULL) {
            return table_size;
        }
        if (hashtable.ht_record[(dest_index + i) % table_size][0] == string[0]) {
            // First char is matching
            full_match = true;
            for (int j = 1; j <= str_len; j++) {
                if (hashtable.ht_record[(dest_index + i) % table_size][j] == '\0') {
                    // Reached the end of the record, check for the end of the search string too
                    if (string[j] != '\0') {
                        full_match = false;
                    }
                    break;
                }
                if (hashtable.ht_record[(dest_index + i) % table_size][j] != string[j]) {
                    // As soon as a non matching char is found, change boolean and break the loop
                    full_match = false;
                    break;
                }
            }
        }
        if (full_match) {
            break;
        }
    }
    // If a match is found, return its index in the table
    // Otherwise, return an error
    return full_match ? (dest_index + i) % table_size : table_size;
}

/* This function removes a string from the hashtable using the ht_find_ind() function
 * Prints an error message if the string isn't found
 */
void ht_remove(char *string) {
    if (!table_exists) {
        printf("No table found. Aborting function.\n");
        return;
    }
    int str_len = find_len(string);
    int dest_index = ht_find_ind(string);
    if (dest_index == table_size) {
        // Error given from ht_find_ind()
        printf("Error, \"%s\" not found in table.\n", string);
    } else {
        for (int i = 0; i < str_len; i++) {
            // Char '^' stands for a tombstone, and it allows the table to be searched after items have been removed
            hashtable.ht_record[dest_index][i] = '^';
        }
        printf("\"%s\" successfully removed from table.\n", string);
    }
}

/* This function is called to search for a given string, printing whether or not it was found,
 * as well as information on its location if it is in the table
 */
bool ht_search(char *string) {
    if (!table_exists) {
        printf("No table found. Aborting function.\n");
        return false;
    }
    int dest_index = ht_find_ind(string);
    if (dest_index == table_size) {
        // Error given from ht_find_ind()
        // printf("\"%s\" not found in table.\n", string);
        return false;
    } else {
        // printf("\"%s\" found at index %d, memory location %p.\n", string, dest_index, (void *)&hashtable.ht_record[dest_index]);
        return true;
    }
}

/* This function frees allocated memory and clears the existing hashtable
 */
void ht_destroy() {
    // Don't destroy a table that doesn't exist
    if (!table_exists) {
        printf("No table found. Aborting function.\n");
        return;
    }
    // Memory needs to be released and the hashtable cleared
    // Release the individual records
    for (int i = 0; i < table_size; i++) {
        if (hashtable.ht_record[i] != NULL) {
            free(hashtable.ht_record[i]);
        }
    }
    // Release the table
    free(hashtable.ht_record);
    table_exists = false;
    printf("Hashtable destroyed.\n");
}
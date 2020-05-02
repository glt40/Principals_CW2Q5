#include <stdio.h>
#import "hashtable.h"

// Initialise vars
char **redactable;
char **filetext;

/*
 * finds the length of a string, given a pointer to the first char, returning an int
 */
int find_length(const char *string) {
    int i = 0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}

/*
 * finds the size, in words, of a file, returns an int, parameter is the filename
 */
int find_size(char *filename) {
    FILE *fp = fopen(filename, "r");
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\n') {
            i++;
        }
    }
    fclose(fp);
    return i;
}

/*
 * strips a string of spaces and transmutes all lower case to upper case, returns the string, parameter is a string
 */
char *strip_upper(char *string) {
    int length = find_length(string);
    char *newstr = malloc(sizeof(char) * (find_length(string) + 1));
    int newstrlen = 0;
    char c;
    // loops through each char and adds them to the new string only if they are letters and all in uppercase
    for (int i = 0; i < length; i++) {
        c = string[i];
        if (c >= 97 && c <= 122) {
            // lowercase, change c to uppercase and put in newstr
            c = c - 32;
            newstr[newstrlen] = c;
            newstrlen++;
        } else if (c >= 65 && c <= 90) {
            // uppercase, put c into newstr
            newstr[newstrlen] = c;
            newstrlen++;
        }
    }
    newstr[newstrlen] = '\0';
    return newstr;
}

/*
 * returns an int array: index 0 contains the char** from the file, index 1 contains the size of the array
 * parameters are filename to read into the array and number of items
 */
char **file_to_arr(char *filename, int items_num) {
    char **array = malloc(sizeof(char *) * items_num);
    FILE *fp = fopen(filename, "r");
    char buff[255];
    // Keep reading until loop is broken
    int c;
    int i = 0;
    int arrindex = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n' || c == ' ') {
            // whitespace
            buff[i] = c;
            buff[i+1] = '\0';
            array[arrindex] = malloc(sizeof(char) * (find_length(buff) + 1));
            for (int j = 0; j <= find_length(buff); j++) {
                array[arrindex][j] = buff[j];
            }
            i = 0;
            arrindex++;
        } else {
            buff[i] = (char)c;
            i++;
        }
    }
    fclose(fp);
    return array;
}

/*
 * prints a string array, parameter is the pointer to the array
 */
void print_final(char **printme, int n) {
    for (int i = 0; i < n; i++) {
        printf("%s", printme[i]);
    }
    printf("\n");
}

/*
 * adds the strings in an array to the hashtable, returns nothing
 * parameters are string array pointer and number of items
 */
void add_hash(char **hashme, int n) {
    // first, create the hashtable to work in
    ht_create();
    for (int i = 0; i < n; i++) {
        ht_add(strip_upper(hashme[i]));
    }
}

/*
 * redacts words found in the hashtable from the input file, returns string array
 * parameters are string array pointer and number of items
 */
char **redact(char **inputfile, int n) {
    char **newfile = malloc(sizeof(char *) * n);
    for (int i = 0; i < n; ++i) {
        char *current = inputfile[i];
        if (ht_search(strip_upper(current))) {
            // match in the text
            newfile[i] = inputfile[i];
            for (int j = 0; j < find_length(newfile[i]) - 1; j++) {
                newfile[i][j] = '*';
            }
        } else {
            // not a match
            newfile[i] = inputfile[i];
        }
    }
    return newfile;
}

/*
 * main function calls other functions
 */
int main() {
    // read in file to array and then hash that array
    int SIZEREDACT = find_size("redact.txt");
    redactable = file_to_arr("redact.txt", SIZEREDACT);
    add_hash(redactable, SIZEREDACT);

    // read in file, redact using the hash search and print the output
    int SIZETEXT = find_size("debate.txt");
    filetext = file_to_arr("debate.txt", SIZETEXT);
    redact(filetext, SIZETEXT);
    print_final(filetext, SIZETEXT);

    // finally, destroy the hashtable
    ht_destroy();
    return 0;
}

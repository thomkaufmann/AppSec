// #include "spell.c"
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>

char *misspelled[MAX_MISSPELLED];
node* hashtable[HASH_SIZE];

int main( int argc, char *argv[] ) 
{

    int count_misspelled = 0;
    FILE * fp;
    const char* testfile;
    const char* wordlist;
    if(argc < 3) 
    {
        printf("Not enough arguments supplied");
    } 
    else 
    {
        testfile = argv[1];
        wordlist = argv[2];
        
        load_dictionary(wordlist, hashtable);
        fp = fopen(testfile, "rb");

        if(fp == NULL)
            printf("file not found\n");
            
        count_misspelled = check_words(fp, hashtable, misspelled);
        fclose(fp);
        printf("Misspelled count:%d\n\n", count_misspelled);
        for(int i = 0; i < count_misspelled; i++)
        {
            printf("Misspelled:%s\n",misspelled[i]);
            free(misspelled[i]);
            misspelled[i] = NULL;
        }    
        // free(misspelled[MAX_MISSPELLED]);    
    }

        return 0;

}
#include "dictionary.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Returns true if all words are spelled correctly, false otherwise. Array misspelled is populated with words that are misspelled.
 */
int check_words(FILE* fp, hashmap_t hashtable[], char * misspelled[])
{
    size_t maxlen = ((LENGTH+1) * sizeof(char));
    int i = 0;
    int p = 0;
    int num_misspelled = 0;
    char * line = NULL;
    char * line2;
    char * word;
    size_t len = 0;
    size_t len_line = 0;
    __ssize_t read = 0; 

    if (fp == NULL)
    {
        //could not open file, so there are no misspellings to report

        //free hashtable and return true
        for (i = 0; i < HASH_SIZE; i++)
        {
            node * cursor = hashtable[i];
            while (cursor != NULL)
            {
                node * temp = cursor;
                cursor = cursor->next;
                free(temp);
            }
        }         
        return 0;
    } 
    else 
    {
        while ((read = getline(&line, &len_line, fp)) != -1) 
        {
            //copy to line2 to avoid altering line which would prevent releasing the line malloc
            line2 = line;

            //remove line return
            line2[strcspn(line2, "\n")] = 0;
            
            //break line2 into words delimited by spaces
            while((word = strsep(&line2, " ")) != NULL)
            {
                len = strlen(word);
                if(len > 0)
                {
                    //remove punctuation from beginning and end of word
                    for (i = 0,p = 0; i < len; i++) {
                        if (! ((i==0 || i==(len-1)) && ispunct(word[i])))
                        {
                            word[p] = word[i];
                            p++;
                        }
                    }   
                    len = p;
                    word[len] = '\0';
                    
                    if(maxlen >= len && num_misspelled < MAX_MISSPELLED && !check_word(word, hashtable))
                    {    
                        //alloc memory in array for misspelled word
                        misspelled[num_misspelled] = (char *)malloc(len+1);

                        //copy word into array
                        if (snprintf(misspelled[num_misspelled], len+1, "%s", word) > len+1)
                        {
                            return -1;    
                        }                        

                        num_misspelled++;
                    }
                }
            }
            free(word);
            word = NULL;
        }
        free(line);
        line = NULL;
        len_line = 0;
    }
    
    //free hashtable
    for (i = 0; i < HASH_SIZE; i++)
    {
        node * cursor = hashtable[i];
        while (cursor != NULL)
        {
            node * temp = cursor;
            cursor = cursor->next;
            free(temp);
        }
    }   
    
    return num_misspelled;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check_word(const char * word, hashmap_t hashtable[]) 
{
    size_t maxlen = ((LENGTH+1) * sizeof(char));
    int len = strlen(word);
    char word1[len];
    int bucket = 0;
    int i = 0;
    int j = 0;
    bool isword = false;
    bool islinked = false;
    node* cursor = NULL;

    if(word != NULL && hashtable != NULL) 
    {
        strncpy(word1,(char *)word, len);
        //remove punctuation from beginning and end of word
        for (i = 0; i < len; i++) {
            //if NOT punctuation on first or last character
            if (! ((i==0 || i==(len-1)) && ispunct(word1[i])))
            {
                word1[j] = word[i];
                j++;
            }
        }   
        len = j;
        word1[len] = '\0'; 

        if(len <= maxlen) 
        {

            //i=0 is the inital test and i=1 is the test after making the word lower case
            for(i = 0; i <= 1; i++) 
            {
                if(i == 1) 
                {
                    //change to lower case
                    for (j = 0; j < len; j++) {
                        word1[j] = tolower(word1[j]);
                    }              
                }

                bucket = hash_function(word1);

                //isword may have been changed to true and there is no need to test again with lower case version
                if(hashtable[bucket] != NULL && !isword) 
                {
                    //a word exists at this point in the hashtable, so compare and check for linked list
                    islinked = false;
                    cursor = hashtable[bucket];

                    do 
                    {
                        if(strncmp(cursor->word,word1,len)==0) 
                        {
                            isword = true;
                        }
                        
                        if(cursor->next != NULL) 
                        {
                            islinked = true;
                            cursor = cursor->next;
                        } 
                        else 
                        {
                            islinked = false;
                        }
                        
                    } while(islinked);
                }            
            }
        }
    }

    return isword;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load_dictionary(const char * dictionary_file, hashmap_t hashtable[]) 
{
    FILE * fp;
    size_t maxlen = ((LENGTH+1) * sizeof(char));
    size_t len = 0;
    __ssize_t read = 0; 
    char * line = NULL;
    int bucket = 0;

    for(int i = 0; i < HASH_SIZE; i++)
    {
        hashtable[i] = NULL;
    }    

    fp = fopen(dictionary_file, "rb");
    if (fp == NULL) {
        return false;
    }

    while ((read = getline(&line, &len, fp)) != -1 && read <= maxlen) 
    {
        node * new_node = (struct node*)malloc(sizeof(struct node));
        if(new_node == NULL) 
        {
            printf("node ptr not null.");
            return false;
        }
        new_node->next = NULL;
        line[strlen(line)-1] = '\0';
        strncpy(new_node->word,line,strlen(line));
    
        bucket = hash_function(new_node->word);

        if(hashtable[bucket] != NULL) {
            new_node->next = hashtable[bucket];
        }
        hashtable[bucket] = new_node;
    }
    free(line);
    line = NULL;
    fclose(fp);

    return true;
}

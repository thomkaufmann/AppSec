#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.c"
#include <string.h>

/**
 * Returns true if all words are spelled correctly, false otherwise. Array misspelled is populated with words that are misspelled.
 */
int check_words(FILE* fp, hashmap_t hashtable[], char * misspelled[])
{
    int count_misspelled = 0;
    char * buffer = 0;
    long length;

    if (fp)
    {
        fseek (fp, 0, SEEK_END);
        length = ftell (fp);
        fseek (fp, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, fp);
        }
    }

    if (buffer)
    {
        char * pch;
        pch = strtok (buffer," ,.-");
        while (pch != NULL)
        {
            if(strlen(pch) > 1 || ((*pch >= 'A' && *pch <= 'Z') || (*pch >= 'a' && *pch <= 'z')))
            {
                // printf("word is:%s:%ld\n",pch,strlen(pch));
                if(!check_word(pch, hashtable))
                {
                    misspelled[count_misspelled] = pch;
                    count_misspelled++;
                }
            }
            pch = strtok (NULL, " ,.-");
        } 
    }   
    // for(int i = 0; i < count_misspelled; i++)
    // {
    //     printf("Misspelled:%s\n",misspelled[i]);
    // }
    if(count_misspelled > 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check_word(const char* word, hashmap_t hashtable[]) 
{
    bool isword = false;
    bool ischained = false;
    if(word != NULL && hashtable != NULL) 
    {
        node* current_node;
        int word_length = strlen(word);
        char lword[word_length+1];
        lword[word_length] = '\0';

        for(int i=0; i<word_length; i++) 
        {
            if(isupper(word[i]))
            {
                lword[i] = tolower(word[i]);
            } 
            else 
            {
                lword[i] = word[i];
            }
        }


        for(int i=0; i<HASH_SIZE; i++) 
        {
            current_node = NULL;
            if(hashtable[i] != NULL) 
            {
                ischained = false;
                current_node = hashtable[i];
                do 
                {
    // printf("word is:%s:node word is:%s:%d\n",word,current_node->word,i);
                    if(strcmp(current_node->word,lword)==0) 
                    {
                        isword = true;
                    }
                // printf("current word:%d:%s\n", i, current_node->word);
                    if(current_node->next != NULL) 
                    {
                        ischained = true;
                        current_node = current_node->next;
                    } 
                    else 
                    {
                        ischained = false;
                    }
                    
                } while(ischained);
            }
        }
    }

    return isword;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[]) 
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(dictionary_file, "r");
    if (fp == NULL)
        return false;

    while ((read = getline(&line, &len, fp)) != -1) 
    {
        node* new_node = malloc(sizeof(node));
        new_node->next = NULL;
        line[strlen(line)-1] = '\0';
        strcpy(new_node->word,line);

        int ht_index = hash_function(new_node->word);

        if(hashtable[ht_index] != NULL)
            new_node->next = hashtable[ht_index];
        hashtable[ht_index] = new_node;
    }

    fclose(fp);

    if (line)
        free(line);

    return true;
}

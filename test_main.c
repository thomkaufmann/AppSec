#include <check.h>
#include "dictionary.h"
#include <stdlib.h>

#define DICTIONARY "wordlist.txt"


START_TEST(test_load_dictionary_success)
{
    //this test verifies that the hashtable is not empty after loading the dictionary
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    node* cursor = NULL;
    int count_entries = 0;   

    //count entries in hashtable
    for(int i = 0; i < HASH_SIZE; i++)
    {
        cursor = hashtable[i];
        if(cursor->word != NULL)
        {
            count_entries++;
            while(cursor->next != NULL)
            {
                cursor = cursor->next;
                count_entries++;
            }
        };
    }   
    ck_assert(count_entries>0);
}
END_TEST

START_TEST(test_load_dictionary_counts)
{
    //this test verifies that the number of items in the dictionary file equals the number of entries in the hashtable
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    node* cursor = NULL;
    FILE *fp;
    int count_items = 0;
    int count_entries = 0;
    char chr;
 
    fp = fopen(DICTIONARY, "rb");
   //extract character from file and store in chr
    chr = getc(fp);
    while (chr != EOF)
    {
        //Count whenever new line is encountered
        if (chr == '\n')
        {
            count_items++;
        }
        //take next character from file.
        chr = getc(fp);
    }
    fclose(fp); //close file.     

    //count entries in hashtable
    for(int i = 0; i < HASH_SIZE; i++)
    {
        cursor = hashtable[i];
        if(cursor->word != NULL)
        {
            count_entries++;
            while(cursor->next != NULL)
            {
                cursor = cursor->next;
                count_entries++;
            }
        };
    }   
    ck_assert(count_items>0 && count_items == count_items);
    // ck_abort_msg ("items:%d:entries:%d\n",count_items, count_entries);   
}
END_TEST

START_TEST(test_check_word_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* correct_word = "Justice";
    const char* incorrect_word = "Caoimhín";
    ck_assert(check_word(correct_word, hashtable));
    ck_assert(!check_word(incorrect_word, hashtable));
}
END_TEST

START_TEST(test_check_word_apostrophe)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* correct_word = "Billy's";
    const char* incorrect_word = "Bill'ys";
    ck_assert(check_word(correct_word, hashtable));
    ck_assert(!check_word(incorrect_word, hashtable));
}
END_TEST

START_TEST(test_check_word_buffer_overflow)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char incorrect_word[500000];
    for(int i = 0; i < 499999; i++)
        incorrect_word[i] = 'A';
    incorrect_word[499999] = 0;
    ck_assert(!check_word(incorrect_word, hashtable));
}
END_TEST

START_TEST(test_check_word_punctuation)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* leading_punctuation = "(leading";
    const char* trailing_punctuation = "trailing!";
    const char* splitting_punctuation = "split/ting!";
    ck_assert(check_word(leading_punctuation, hashtable));
    ck_assert(check_word(trailing_punctuation, hashtable));
    ck_assert(!check_word(splitting_punctuation, hashtable));
}
END_TEST

START_TEST(test_check_word_capitalization)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* correct_word = "Freight";
    ck_assert(check_word(correct_word, hashtable));
    correct_word = "FREIGHT";
    ck_assert(check_word(correct_word, hashtable));
}
END_TEST

START_TEST(test_check_words_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    const char * file = "test1.txt";
    FILE *fp = fopen(file,"w");

    fprintf(fp,"%s\n","This text should produce zero misspelled words.");
    fclose(fp);
    fp = fopen(file,"rb");
    ck_assert(check_words(fp, hashtable, misspelled) == 0);
    fclose(fp);
    remove(file);
}
END_TEST

START_TEST(test_check_words_errors)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    const char * file = "test1.txt";
    FILE *fp = fopen(file,"w");

    fprintf(fp,"%s\n","This text shoold produse three misspelled wordz.");
    fclose(fp);
    fp = fopen(file,"rb");
    ck_assert(check_words(fp, hashtable, misspelled) == 3);
    ck_assert(strncmp("produse",misspelled[1],7)==0);
    fclose(fp);
    remove(file);
}
END_TEST

START_TEST(test_check_words_blank)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    const char * file = "test1.txt";
    FILE *fp = fopen(file,"w");

    fprintf(fp,"\n");
    fclose(fp);
    fp = fopen(file,"rb");
    ck_assert(check_words(fp, hashtable, misspelled) == 0);
    fclose(fp);
    remove(file);
}
END_TEST

Suite *
check_word_suite(void)
{
    Suite * suite;
    TCase * check_word_case;
    suite = suite_create("check_word");
    check_word_case = tcase_create("Core");
    tcase_add_test(check_word_case, test_check_word_normal);
    tcase_add_test(check_word_case, test_check_word_buffer_overflow);
    tcase_add_test(check_word_case, test_check_word_punctuation);
    tcase_add_test(check_word_case, test_check_word_apostrophe);
    tcase_add_test(check_word_case, test_check_word_capitalization);
    suite_add_tcase(suite, check_word_case);

    return suite;
}


Suite *
check_words_suite(void)
{
    Suite * suite;
    TCase * check_words_case;
    suite = suite_create("check_words");
    check_words_case = tcase_create("Core");
    tcase_add_test(check_words_case, test_check_words_normal);
    tcase_add_test(check_words_case, test_check_words_errors);
    tcase_add_test(check_words_case, test_check_words_blank);
    suite_add_tcase(suite, check_words_case);

    return suite;
}

Suite *
load_dictionary_suite(void)
{
    Suite * suite;
    TCase * load_dictionary_case;
    suite = suite_create("load_dictionary");
    load_dictionary_case = tcase_create("Core");
    tcase_add_test(load_dictionary_case, test_load_dictionary_success);
    tcase_add_test(load_dictionary_case, test_load_dictionary_counts);
    suite_add_tcase(suite, load_dictionary_case);

    return suite;
}

int
main(void)
{
    int failed;
    Suite *suite;
    SRunner *runner;
    
    suite = load_dictionary_suite();
    runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    suite = check_word_suite();
    runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    suite = check_words_suite();
    runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
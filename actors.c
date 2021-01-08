#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "hashfn.h"

int main(int argc, char *argv[]){

    //open all files for reading in binary
    FILE *title_basics_vhs = fopen("title.basics.vhs", "rb");
    FILE *title_basics_khs = fopen("title.basics.khs", "rb");
    FILE *title_basics_kv = fopen("title.basics.kv", "rb");
    FILE *title_principals_khs = fopen("title.principals.khs", "rb");
    FILE *title_principals_vhs = fopen("title.principals.vhs", "rb");
    FILE *title_principals_kv = fopen("title.principals.kv", "rb");
    FILE *name_basics_khs = fopen("name.basics.khs", "rb");
    FILE *name_basics_vhs = fopen("name.basics.vhs", "rb");
    FILE *name_basics_kv = fopen("name.basics.kv", "rb");

    //look up the hash value in the title.basics.vhs file
    int capacity_titlebasics = get_capacity(title_basics_vhs);
    int hashval_titlebasics = hashfn(argv[1], capacity_titlebasics - 1);
    fseek(title_basics_vhs, 0, SEEK_SET);
    int len = strlen(argv[1]);
    
    int id;
    char title_basics_key[STRLEN] = "";
    char title_basics_value[STRLEN] = "";

    //searches the title basics file for the corresponding movie code for the given movie name
    for(int i = hashval_titlebasics; i < capacity_titlebasics; i++){
        read_index(title_basics_vhs, i, &id);
        if(id != -1){
            read_val(title_basics_kv, id, title_basics_value);
        }
        if(strncmp(title_basics_value, argv[1], len) == 0){
            read_key(title_basics_kv, id, title_basics_key);
            break;
        }
        else if(i == hashval_titlebasics - 1){
            break;
        }
        else if(i == capacity_titlebasics - 1){
            i = -1;
        }
    }

    int hashval_titleprincipals = hashfn(title_basics_key, capacity_titlebasics - 1);
    char title_principals_key[STRLEN];
    char title_principals_value[STRLEN] = "";
    char name_basics_key[STRLEN] = "";
    char name_basics_value[STRLEN];
    int id2;

    //uses the previously found movie code to find the corresponding actor codes
    for(int i = hashval_titleprincipals; i < capacity_titlebasics; i++){
        read_index(title_principals_khs, i, &id);
        if(id != -1){
            read_key(title_principals_kv, id, title_principals_key);
        }
        if(strncmp(title_basics_key, title_principals_key, 256) == 0) {
            read_val(title_principals_kv, id, title_principals_value);
            unsigned int namebasics_hash = hashfn(title_principals_value, capacity_titlebasics - 1);            
            
            //uses the corresponding actor codes to print out all of the actors names that appear in the selected movie
            for(int j = namebasics_hash; j < capacity_titlebasics; j++) {
                read_index(name_basics_khs, j, &id2);
                if(id2 != -1) {
                    read_key(name_basics_kv, id2, name_basics_key);
                }
                if(strcmp(name_basics_key, title_principals_value) == 0) {
                    read_val(name_basics_kv, id2, name_basics_value);
                    printf("%s\n", name_basics_value);
                    break;
                }
                else if(j == capacity_titlebasics -1) {
                    j = -1;
                }
                else if(j == namebasics_hash - 1) {
                    break;
                } 
            }
        }
        else if(i == hashval_titleprincipals - 1){
            break;
        }
        else if(i == capacity_titlebasics - 1){
            i = -1;
        }
    }

    //close all of the files open for reading to prevent memory leaks
    fclose(title_basics_vhs);
    fclose(title_basics_khs);
    fclose(title_basics_kv);
    fclose(title_principals_vhs);
    fclose(title_principals_khs);
    fclose(title_principals_kv);
    fclose(name_basics_vhs);
    fclose(name_basics_khs);
    fclose(name_basics_kv);

    return 0;
}
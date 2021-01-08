#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "hashfn.h"

int main(int argc, char *argv[]){

    //if a different number of command line arguments are provided, prints the stderr and returns
    if(argc != 3){
        fprintf(stderr, "Usage: %s filename.kv capacity\n", argv[0]);
        return -1;
    }

    //otherwise, the first argument is interpreted as a kv file
    else{

        //declares file pointers for kv, khs, and vhs files
        FILE *kv;
        FILE *khs;
        FILE *vhs;
        long capacity = atol(argv[2]);

        //determines the name of the given file and removes the .kv extension
        int lengthName = strlen(argv[1]) - 3;
        char *kvFile = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 1));
        char *fileName = (char *)malloc(sizeof(char) * (lengthName + 1));
        strcpy(kvFile, argv[1]);
        strncpy(fileName, kvFile, lengthName);
        fileName[lengthName] = '\0';

    
        //defines extensions for khs and vhs files
        char *khsFile = (char *)malloc(sizeof(char) * (lengthName + 5));
        char *vhsFile = (char *)malloc(sizeof(char) * (lengthName + 5));

        char khsEx[5] = ".khs\0";
        char vhsEx[5] = ".vhs\0";

        //creates names for khs and vhs files
        strcpy(khsFile, fileName);
        strcat(khsFile, khsEx);
        strcpy(vhsFile, fileName);
        strcat(vhsFile, vhsEx);

        //opens the kv file for reading and the khs+vhs files for reading+writing
        kv = fopen(kvFile, "rb");
        khs = fopen(khsFile, "wb+");
        vhs = fopen(vhsFile, "wb+");

        //khs and vhs files are initialized by calling the write_empty function
        write_empty(khs, capacity);
        write_empty(vhs, capacity);

        char key[STRLEN];
        char val[STRLEN];
        int rKeyVal = 0;
        int kvCapacity = (get_capacity(kv) * 4)/512;
        
        fseek(kv, 0, SEEK_SET);
        for(int i = 0; i < kvCapacity; i++){

            rKeyVal = read_keyval(kv, key, val);
            if(rKeyVal == 2){
                int hashVal = hashfn(val, capacity - 1);
                int hashKey = hashfn(key, capacity - 1);
               
                int read;
                for(int j = hashKey; j < capacity; j++){
                    fseek(khs, j*sizeof(int), SEEK_SET);
                    fread(&read, sizeof(int), 1, khs);
                    if(read == -1){
                        write_index(khs, i, j);
                        break;
                    }
                    else if(j == (capacity -1)){
                        j = -1;
                    }
                    else if(j == (hashKey -1)){
                        break;
                    }
                }
                for(int j = hashVal; j < capacity; j++){
                    fseek(vhs, j*sizeof(int), SEEK_SET);
                    fread(&read, sizeof(int), 1, vhs);
                    if(read == -1){
                        write_index(vhs, i, j);
                        break;
                    }
                    else if(j == (capacity -1)){
                        j = -1;
                    }
                    else if(j == (hashVal -1)){
                        break;
                    }
                }
            }
        }

        //frees any allocated memory and closes all files
        fclose(kv);
        fclose(khs);
        fclose(vhs);
        free(kvFile);
        free(fileName);
        free(khsFile);
        free(vhsFile);

    }
    return 0;
}
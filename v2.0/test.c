#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char **str_box = (char**)malloc(2 * sizeof(char*));
    for(int i = 0; i < 7; i++){
        str_box[i] = (char*)malloc(7 * sizeof(char));
    }
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 7; j++){
            if(j == 7 - 1){
                str_box[i][j] = '\0';
                continue;
            }
            if(j == 7 - 2){
                str_box[i][j] = '\n';
                continue;
            }
            str_box[i][j] = 'a' + j;
        }
    for(int i = 0; i < 2; i++)
        printf("%s",str_box[i]);
    printf("-----");
    char *str_box_all = (char*)malloc(((2*7)+1)*sizeof(char));
    strcat(str_box_all, str_box[0]);
    strcat(str_box_all, str_box[1]);
    printf("%s",str_box_all);
    printf("-----\n");
    str_box[0][4] = 'X';
    str_box_all[1 * 5 - 1] = 'X';
    for(int i = 0; i < 2; i++)
        printf("%s",str_box[i]);
    printf("%s",str_box_all);
    return 0;
}
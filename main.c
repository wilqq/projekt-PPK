#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LENGTH 100

typedef struct u_element { //definicja typu elementu listy podwieszanej
   struct u_element *next;
   char line[LENGTH];
 } u_list_el;

typedef struct element { //definicja typu elementu listy
    struct element *next;
    char label[LENGTH];
    u_list_el *under;
 } list_el;

int get_files_names(int argc, char ** argv, char **input_file_name, char **output_file_name);
int read_file(FILE* plik, list_el** head);
void add_to_list(list_el** head, char *word, char *author, char *title);
void add_to_u_list(list_el* label_head, char *author, char *title);
void clean_memory(list_el** head);
void clean_u_memory(list_el* label_head);
void save_list(char *file_name, list_el *head);
void save_u_list(FILE* output, u_list_el *u_head);
void strip(char *str);
int check_filename_ext(char *filename);


int main(int argc, char ** argv)
{
    list_el *head = NULL;
    FILE *input;
    char *input_file_name, *output_file_name;

    if(get_files_names(argc, argv, &input_file_name, &output_file_name) != 0) {
        printf("Niewlasciwe parametry programu!\n");
        return 0;
    }

    if(check_filename_ext(input_file_name) != 0) {
        printf("Plik %s ma niewlasciwe rozszerzenie!\n", input_file_name);
        return 0;
    }

    input = fopen(input_file_name, "r");

    if(input == NULL) {
        printf("Nie udalo sie otworzyc pliku %s!\n", input_file_name);
        return 0;
    }
    if(read_file(input, &head) == 0) {
        save_list("wy.txt", head);
    } else {
        printf("Plik ma niepoprawny format!\n");
    }
    fclose(input);
    clean_memory(&head);

    return 0;
}

int get_files_names(int argc, char ** argv, char **input_file_name, char **output_file_name)
{
    int i, input_set = 0, output_set = 0;
    for(i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0) {
            *input_file_name = argv[++i];
            input_set = 1;
            continue;
        }

        if(strcmp(argv[i], "-o") == 0) {
            *output_file_name = argv[++i];
            output_set = 1;
        }
    }

    if(input_set == 1 && output_set == 1)
        return 0;

    return -1;
}

int read_file(FILE* plik, list_el** head)
{
    char *token;
    char author[LENGTH], title[LENGTH], labels[LENGTH], label[LENGTH];
    int a_read, t_read, l_read;

    while(!feof(plik)) {
        a_read = fscanf(plik, "%[^;];", author);
        t_read = fscanf(plik, "%[^;];", title);
        l_read = fscanf(plik, "%[^\n];", labels);
        fscanf(plik, "\n");

        strip(author);
        strip(title);
        strip(labels);

        if(a_read == 0 || t_read == 0 || l_read == 0)
            return -1;

        if(strcmp(author, "") == 0 || strcmp(title, "") == 0 || strcmp(labels, "") == 0)
            return -1;


        token = strtok(labels, ", ");
        while(token != NULL) {
            strcpy(label, token);
            add_to_list(head, label, author, title);
            token = strtok(NULL, ", ");
        }
    }
    return 0;
}

void add_to_list(list_el** head, char *word, char *author, char *title)
{
    list_el *wsk, *prev, *new_el;
    if((*head) == NULL) {
        new_el = malloc (sizeof(list_el));
        strcpy(new_el->label, word);
        new_el->next = NULL;
        new_el->under = NULL;
        *head = new_el;
    }
    else {
        if(strcmp((*head)->label, word) > 0) {
            new_el = malloc (sizeof(list_el));
            new_el->under = NULL;
            strcpy(new_el->label, word);
            new_el->next = *head;
            *head = new_el;
        }
        else {
            wsk = *head;
            while (wsk != NULL && strcmp(wsk->label, word) < 0) {
                prev = wsk;
                wsk = wsk->next;
            }
            if(wsk == NULL || strcmp(wsk->label, word)) {
                new_el = malloc (sizeof(list_el));
                new_el->under = NULL;
                strcpy(new_el->label, word);
                new_el->next = wsk;
                prev->next = new_el;
            }
            else
                new_el = wsk;
        }
    }
    add_to_u_list(new_el, author, title);
}

void add_to_u_list(list_el* label_head, char *author, char *title)
{
    char line[LENGTH];
    strcpy(line, author);
    strcat(line, "; ");
    strcat(line, title);
    u_list_el *wsk, *prev, *new_el;

    new_el = malloc (sizeof(u_list_el));
    strcpy(new_el->line, line);
    wsk = label_head->under;

    if(wsk == NULL) {
        new_el->next = NULL;
        label_head->under = new_el;
    } else if (strcmp(wsk->line, line) > 0) {
        new_el->next = wsk;
        label_head->under = new_el;
    } else {
        while (wsk != NULL && strcmp(wsk->line, line) < 0) {
            prev = wsk;
            wsk = wsk->next;
        }
        new_el->next = wsk;
        prev->next = new_el;
    }
}

void clean_memory(list_el** head)
{
    list_el *wsk, *prev;
    wsk = *head;
    while(wsk != NULL) {
        prev = wsk;
        wsk = wsk->next;
        clean_u_memory(prev);
        free(prev);
    }
    *head = NULL;
}

void clean_u_memory(list_el* label_head)
{
    u_list_el *wsk, *prev;
    wsk = label_head->under;
    while(wsk != NULL) {
        prev = wsk;
        wsk = wsk->next;
        free(prev);
    }
}


void save_list(char *file_name, list_el *head)
{
    FILE *output;

    if(check_filename_ext(file_name) != 0) {
        printf("Plik ma %s niewlasciwe rozszerzenie!\n", file_name);
        return;
    }

    output = fopen(file_name, "w");

    if(output == NULL) {
        printf("Nie udalo sie zapisac do pliku %s!\n", file_name);
        return;
    }

    list_el *wsk = head;
    while( wsk != NULL ) {
        fprintf(output, "%s\n", wsk->label);
        save_u_list(output, wsk->under);
        fprintf(output, "\n");
        wsk = wsk->next;
    }
    fclose(output);
    printf("Dane zostaly zapisane do pliku %s\n", file_name);
    return;
}

void save_u_list(FILE* output, u_list_el *u_head)
{
    u_list_el *wsk = u_head;
    while( wsk != NULL ) {
        fprintf(output, "%s\n", wsk->line);
        wsk = wsk->next;
    }
}

void strip(char *str)
{
    char  *tmp = str;
    int length = strlen(tmp);

    while(isspace(tmp[length - 1]))
        tmp[--length] = 0;

    while(*tmp && isspace(*tmp)) {
        tmp++;
        length--;
    }

    memmove(str, tmp, length + 1);
}

int check_filename_ext(char *filename)
{
    char *dot = strrchr(filename, '.');
    if(strcmp(dot, ".txt") == 0)
        return 0;
    else
        return -1;
}

#include <stdio.h>
#include <stdlib.h>


#define LENGTH 100

typedef struct u_element {
   struct u_element *next;
   char line[LENGTH];
 } u_list_el;

typedef struct element {
    struct element *next;
    char label[80];
    u_list_el *under;
 } list_el;



int main()
{
    list_el *head = NULL;
    FILE *input;
    input = fopen("we.txt", "r");

    if(input != NULL) {
        read_file(input, &head);
        fclose(input);
        save_list("wy.txt", head);
    }
    else
        printf("Nie udalo sie otworzyc pliku!\n");

    return 0;
}

void read_file(FILE* plik, list_el** head)
{
    char *token;
    char line[LENGTH], author[LENGTH], title[LENGTH], labels[LENGTH], label[LENGTH];
    while(!feof(plik)) {
        fscanf(plik, "%[^;];", author);
        fscanf(plik, "%[^;];", title);
        fscanf(plik, "%[^\n];", labels);
        fscanf(plik, "\n");

        token = strtok(labels, ", ");
        while(token != NULL) {
            strcpy(label, token);
            add_to_list(head, label, author, title);
            token = strtok(NULL, ", ");
        }
    }
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
    print_list(*head);
}

void add_to_u_list(list_el* label_head, char *author, char *title)
{
    char line[LENGTH];
    strcpy(line, author);
    strcat(line, ";");
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

void save_list(char *file_name, list_el *head)
{
    FILE *output;
    output = fopen(file_name, "w");
    list_el *wsk = head;
    while( wsk != NULL ) {
        fprintf(output, "%s\n", wsk->label);
        save_u_list(output, wsk->under);
        fprintf(output, "\n");
        wsk = wsk->next;
    }
}

void save_u_list(FILE* output, u_list_el *u_head)
{
    u_list_el *wsk = u_head;
    while( wsk != NULL ) {
        fprintf(output, "%s\n", wsk->line);
        wsk = wsk->next;
    }
}


void print_list(list_el *head)
{
    printf("\n");
    list_el *wsk = head;
    while( wsk != NULL ) {
        printf ("-%s-\n", wsk->label);
        print_u_list(wsk->under);
        wsk = wsk->next;
    }
}

void print_u_list(u_list_el *u_head)
{
    u_list_el *wsk = u_head;
    while( wsk != NULL ) {
        printf ("- -%s-\n", wsk->line);
        wsk = wsk->next;
    }
}

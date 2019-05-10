#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.c"

void TestSingleUnhash(char *hashed, char *wanted)
{
    folder_t *fold = malloc(sizeof(struct folder));
    fold->stpt = 1;
    fold->inc = 1;
    fold->hhash = hashed;
    fold->res = malloc(4);
    fold->found = 0;
    revhash(fold);
    char *ttmp = fold->res;
    ttmp[fold->tmpl] = '\0';
    fold -> res = ttmp;
    if(strcmp(fold->res,wanted)== 0)
    {
        printf("\nUnhash function works:\nReturned value: %s   Expected value: %s\n", fold->res, wanted);
    }
    else
    {
        printf("\nUnhash function doesn't work:\nReturned value:%s  Expected value:%s\n", fold->res, wanted);
    }
}


void TestRemovei(struct list *list, struct list *listExpected)
{
    removei(list);
    if(list->first->ptr == listExpected->first->ptr && list->first->lettercount == listExpected->first->lettercount)
    {
        printf("\nRemovei function works:\nReturned list ptr:%d  lettercount:%d\nExpected list ptr:%d  lettercount:%d\n", list->first->ptr, list->first->lettercount, listExpected->first->ptr, listExpected->first->lettercount);
    }
    else
    {
        printf("\nRemovei function doesn't works:\nReturned list ptr:%d  lettercount:%d\nExpected list ptr:%d  lettercount:%d\n", list->first->ptr, list->first->lettercount, listExpected->first->ptr, listExpected->first->lettercount);
    }
}

void TestAppendl(struct list *lista, struct list *listb, struct list *listExpected)
{
    appendl(lista, listb);
    int counter = 0;
    if(listExpected->size == lista->size)
    {
        printf("\nAppendl works\n");
    }
    else
    {
        printf("\nAppendl doesn't work %d   %d\n", counter, lista->size);
    }
}

void TestLetcountC(char *word, int expected)
{
    int value = letcount(word, 1);
    if(value == expected)
    {
        printf("\nletcount works: Returned value: %d   Expected value: %d\n", value ,expected);
    }
    else
    {
        printf("\nletcount doesn't works: Returned value: %d   Expected value: %d\n", value ,expected);
    }
}

void TestLetcountV(char *word, int expected)
{
    int value = letcount(word, 0);
    if(value == expected)
    {
        printf("\nletcount works: Returned value: %d   Expected value: %d\n", value ,expected);
    }
    else
    {
        printf("\nletcount doesn't works: Returned value: %d   Expected value: %d\n", value ,expected);
    }
}

int main(int argc, char *argv[])
{

    char *name4c = malloc(4);
    char *argg[argc];
    int i;
    for(i = 0; i<argc; i++)
    {
        char *argreader = argv[i];
        int l = strlen(argreader);
        if(argreader[l-1] == 'n' && argreader[l-2] == 'i' && argreader[l-3] == 'b' && argreader[l-4] == '.' )
        {
            name4c = argreader;
        }
    }

    FILE *fp;
    fp = fopen(name4c, "rb");
    if(fp == NULL)
    {
        printf("File can't be opened");
        return 0;
    }
    char *testhash4c = readit(fp);
    char *wanted4c = "otus";
    fclose(fp);
    TestSingleUnhash(testhash4c, wanted4c);


    struct node *heada = malloc(sizeof(struct node));
    heada->lettercount = 0;
    heada->ptr = 1111;
    struct list *lista = malloc(sizeof(struct list));
    lista->first = heada;
    lista->size = 1;
    newnode(lista, 555, 0);
    newnode(lista, 556, 0);
    struct node *headb = malloc(sizeof(struct node));
    headb->lettercount = 0;
    headb->ptr = 1111;
    struct list *listExpected = malloc(sizeof(struct list));
    listExpected->first = headb;
    listExpected->size = 1;
    TestRemovei(lista, listExpected);


    newnode(listExpected, 555, 0);
    newnode(listExpected, 556, 0);
    newnode(listExpected, 557, 0);
    struct node *headc = malloc(sizeof(struct node));
    headc->lettercount = 0;
    headc->ptr = 1111;
    struct list *listb = malloc(sizeof(struct list));
    listb->first = headc;
    listb->size = 1;
    newnode(listb, 557, 0);

    TestAppendl(lista, listb, listExpected);


    char *testcharC = "asterix";  /// C = 4;
    TestLetcountC(testcharC, 4);
    char *testcharV = "obelix";  /// V = 3;
    TestLetcountV(testcharV, 3);


}




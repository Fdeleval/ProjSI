#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sha256.c"
#ifdef unix
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#endif

typedef struct folder
{
    int stpt;
    int resset;
    int inc;
    int ff;
    char *hhash;
    int found;
    char *res;
    int max;
    int tmpl;
    int tcnt;
    int lcnt;
    struct list *list;
}folder_t;

struct node
{
    int ptr; /// pointer to the password
    int lettercount; /// amount of valuable letters in the password
    struct node *next;
};

struct list
{
    struct node *first;
    int size;
};


int printl(struct list *list)
{
    if(list->first->ptr == 1111)
    {
        return 0;
    }
    char *candi = (char*) list->first->ptr;
    printf("%s    %d\n", candi, list->first->lettercount);
    list->first = list->first->next;
    printl(list);
}

int removei(struct list *list)
{
    if(list->first->ptr == 1111)
    {
        return 0;
    }
    ///printf("%d\n", list->first->lettercount);
    struct node *tofree = list->first;
    list->first = list->first->next;
    free(tofree);
    removei(list);
}

int newnode(struct list *list, int ptr, int lettercount)
{
    struct node *newlink = malloc(sizeof(struct node));

    newlink-> ptr = ptr;
    newlink-> lettercount = lettercount;
    newlink-> next = list -> first;

    list -> first = newlink;
    list -> size = (list -> size) +1;
    return 0;
}

int appendl(struct list *lista, struct list *listb)
{
    for(;;)
    {
        int lettercount = listb->first->lettercount;
        int ptr = listb->first->ptr;
        if(ptr != 1111)
        {
            newnode(lista, ptr, lettercount);
            listb->first = listb->first->next;
        }
        else
        {
            return 0;
        }
    }
}

void* compare2(struct list *lista, struct list *listb)
{
    if(listb->first->lettercount > lista->first->lettercount)
    {
        return listb;
    }
    if(listb->first->lettercount < lista->first->lettercount)
    {
        return lista;
    }
    else
    {
        appendl(lista, listb);
        return lista;
    }
}

void* comparen(struct list *list[], int n)
{
    if(n==1)
    {
        return list[0];
    }
    if(n%2 == 0)
    {
        int newn = n/2;
        struct list *newlist[newn];
        int i;
        for(i = 0; i<newn; i++)
        {
            newlist[i] = compare2(list[i], list[i+1]);
        }
        comparen(newlist, newn);
    }
    else
    {
        int newn = (n+1)/2;
        struct list *newlist[newn];
        int i;
        for(i = 0; i<newn; i++)
        {

            if(i == 0)
            {
                newlist[i] = list[i];
            }
            else
            {
                newlist[i] = compare2(list[i], list[i+1]);
            }
        }
        comparen(newlist, newn);
    }
}

int letcount(char *arg, int voc)
{
    int counte = 0;
    char *voy = malloc(4);
    if(voc == 0)
    {
        voy = "aeiouy";
    }
    else
    {
        voy = "bcdfghjklmnpqrstvwxz";
    }

    int i;
    int j;
    for(i = 0;i<strlen(arg);i++)
    {
        for(j = 0;j<strlen(voy);j++)
        {
            if(arg[i] == voy[j])
                counte++;
        }
    }
    return counte;
}

char* left(int x, int pos, char *tmp)
{
    char *lett = "abcdefghijklmnopqrstuvwxyz";
    if(x <27)
    {
        tmp[pos] = lett[x-1];
        tmp[pos+1] = '\0';
        return strrev(tmp);
    }
    else
    {
        int div = x/26;
        int mod = x%26;
        int num = x - (div*26);
        if(mod != 0)
        {
            tmp[pos] = lett[num-1];
            left(div, pos+1, tmp);
        }
        else
        {
            tmp[pos] = lett[25];
            left(div-1, pos+1, tmp);
        }
    }
}


int revhash(void *arg)
{
    folder_t *fold = (folder_t *) arg;
    int i = fold->stpt;
    int tnumber = fold -> tcnt;

    for(;fold->found != 1;)
    {
        int k;
        for(k = 0;fold->found != 1;k++)
        {

            uint8_t hashres[SHA256_DIGEST_SIZE];
            char tmmp[16];
            char *tmp = left(i, 0, tmmp);
            sha256_buffer(tmp, strlen(tmp), hashres);
            if (memcmp(fold->hhash, hashres, SHA256_DIGEST_SIZE) == 0)
            {
                fold->tmpl = strlen(tmp);
                strncpy(fold->res, tmp, strlen(tmp));
                fold->found = 1;
                ///printf("%s   ", tmp);
                return 0;
            }
            //printf("tnumber:%d     i:%d\n", tnumber, i);
            //sleep(0.5);
            i=i+ fold->inc;
        }
    }
}


char* readit(FILE *fp)
{
    char *hashed = malloc(32);
    fread(hashed, 32, 1, fp);

    return hashed;
}



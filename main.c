#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "sha256.c"
#ifdef unix
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#endif
#define MAX_CORES 16
#define rows 1
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;
int acount = 0;
int voc = 0;
int tcount = 1;
void* threadhash(void *arg);
void* threadhashkid(void *arg);
int revhashkid(void *arg);

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
    int ptr;
    int lettercount; 
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
    fprintf(stdout, "%s\n", candi);
    list->first = list->first->next;
    printl(list);
}

int removei(struct list *list)
{
    if(list->first->ptr == 1111)
    {
        return 0;
    }
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

int letcount(char *arg)
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
    int kmax = 475254;

    for(;fold->found != 1;)
    {
        int k;
        for(k = 0; k<kmax && fold->found != 1;k++)
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
                return 0;
            }
            i=i+ fold->inc;
            if(tcount==0)
                kmax = 1000000000;
            else
                kmax = 475254;
        }
        if(acount == fold->max && tcount > 0 && fold->found != 1)
        {
            i = i-fold->inc;
            pthread_t hhelp;
            fold->tcnt = fold->tcnt + 1;  
            fold->inc = fold->tcnt+1;       
            fold->stpt = i;
            fold->resset++;
            pthread_create(&hhelp, NULL, threadhashkid, fold);
            tcount--;
            i = i+fold->inc;
        }
    }
}

int revhashkid(void *arg)
{
    folder_t *fold = (folder_t *) arg;
    int mynumber = fold->tcnt;
    int reboot = fold->resset;
    int i = fold->stpt;
    int tnumber = fold -> tcnt;
    for(;fold->found != 1;)
    {
        int i = fold->stpt + fold->inc - mynumber;
        reboot = fold->resset;
        while(reboot == fold->resset)
        {
            uint8_t hashres[SHA256_DIGEST_SIZE];
            char tmmp[16];
            char *tmp = left(i, 0, tmmp);
            sha256_buffer(tmp, strlen(tmp), hashres);
            if (memcmp(fold->hhash, hashres, SHA256_DIGEST_SIZE) == 0)
            {
                fold->tmpl = strlen(tmp);
                strncpy(fold->res, tmp, strlen(tmp));
                return 0;
            }
            i=i+ fold->inc;
        }
    }
}


void* threadhash(void *arg)
{
    folder_t *fold = (folder_t *) arg;
    revhash(fold);
    char *ttmp = fold->res;
    ttmp[fold->tmpl] = '\0';
    fold -> res = ttmp;
    fold->lcnt = letcount(ttmp);
    pthread_mutex_lock(&mutex);

        int lett = letcount(ttmp);
        int pptr = (int) (long) &ttmp[0];
        if(lett == fold->list->first->lettercount)
        {
            newnode(fold->list, pptr, lett);
        }
        if(lett> fold->list->first->lettercount)
        {
            removei(fold->list);
            newnode(fold->list, pptr, lett);
        }

    pthread_mutex_unlock(&mutex);
    counter++;
    tcount++;
    free(fold);
    return NULL;
}

void* threadhashkid(void *arg)
{
    folder_t *fold = (folder_t *) arg;
    revhashkid(fold);
    char *ttmp = fold->res;
    ttmp[fold->tmpl] = '\0';
    fold -> res = ttmp;
    fold->found = 1;
    tcount++;
    return NULL;
}


char* readit(FILE *fp)
{
    char *hashed = malloc(32);
    fread(hashed, 32, 1, fp);

    return hashed;
}

int main(int argc, char *argv[])
{
    int numberoffiles = 0;
    int i;
    int maxthreads = 1;
    int VoC = 0;
    char *argg[argc];
    int j = 0;
    for(i = 0; i<argc; i++)
    {
        char *argreader = argv[i];
        int l = strlen(argreader);
        if(argreader[l-1] == 't' && argreader[l-2] == '-')
        {
            maxthreads = atoi(argv[i+1]);
            tcount = maxthreads;
        }
        else if(argreader[l-1] == 'c' && argreader[l-2] == '-')
        {
            voc = 1;
        }
        else if(argreader[l-1] == 'n' && argreader[l-2] == 'i' && argreader[l-3] == 'b' && argreader[l-4] == '.' )
        {
            argg[j] = argreader;
            j++;
            numberoffiles++;
        }
    }


    struct node *head[numberoffiles];
    struct list *list[numberoffiles];
    int r;
    for(r = 0; r<numberoffiles ; r++)
    {
        head[r] = malloc(sizeof(struct node));
        head[r]->lettercount = 0;
        head[r]->ptr = 1111;
        list[r] = malloc(sizeof(struct list));
        list[r]->first = head[0];
        list[r]->size = 1;
    }
    if(numberoffiles == 0)
    {
        printf("No files detected!\n");
        return 1;
    }
    int s;
    for(s = 0; s<numberoffiles;s++)
    {
        counter = 0;
        acount = 0;

        FILE *fp;
        fp = fopen(argg[s], "rb");
        if(fp == NULL)
        {
            printf("File can't open\n");
            break;
        }
        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        int max = size/32;
        for(;counter < max;)
        {
            if(tcount != 0 && acount<max)
            {
                folder_t *fold = malloc(sizeof(struct folder));
                fold->stpt = 1;
                fold->inc = 1;
                fold->ff = 0;
                fold->resset = 0;
                fold->hhash = readit(fp);
                fold->res = malloc(4);
                fold->found = 0;
                fold->max = max;
                fold->tcnt = 0;
                fold->list = list[s];
                pthread_t unhasher;
                pthread_create(&unhasher, NULL, threadhash, fold);
                acount++;
                tcount--;
            }
        }
        fclose(fp);
    }
    printf("\n\n         Candidates:\n\n");
    struct list *liist = comparen(list, numberoffiles);
    printl(liist);
    return 0;
}

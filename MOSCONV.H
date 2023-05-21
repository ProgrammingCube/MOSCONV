#ifndef MAIN_H
#define MAIN_H


#include "stdio.h"

typedef struct srcstr
{
    /*:20020000A000B91102F00720478AC84C02024C00804865792C20416D616E64612E2049203B*/
    unsigned char rec_strt;
    unsigned short data_len;
    unsigned short address;
    unsigned short data_typ;
    unsigned char data[MAXREC];
    unsigned char chksum;
} srcstr;

typedef struct deststr
{
    unsigned char rec_strt;
    unsigned short data_len;
    unsigned short address;
    unsigned char data[24];
    unsigned short chksum;
} deststr;


#if AZTEC_C == 1

void* memcpy(d, s, n)
void* d;
void* s;
unsigned char n;
{
    while (n > 0)
    {
        *(unsigned char*)d = *(unsigned char*)s;
	((unsigned char*)(d))++;
	((unsigned char*)(s))++;
	n--;
    }
    return d;
}


unsigned char strlen(line)
unsigned char *line;
{
    unsigned char i = 0;
    unsigned char a = 1;
    while (a != '\0')
    {
        a = line[i];
        i++;
    }
    return i - 1;
}

void* memset(s, c, len)
void *s;
unsigned char c;
int len;
{
    unsigned char *dst = s;
    while (len > 0)
    {
        *dst = c;
        dst++;
        len--;
    }
    return s;
}
#endif /* AZTEC_C */

void datacopy(line, dataline, len)
unsigned char *line;
unsigned char *dataline;
unsigned char len;
{
    unsigned char i;
    for (i = 0; i < len; i++)
        dataline[i] = line[i + 9];
}

unsigned char charNum(c)
unsigned char c;
{
    if (c < 58)
        return c - 48;
    return c - 55; /*- 'A' + 9 */
}

unsigned char charByte(c, d)
unsigned char c;
unsigned char d;
{
    return (charNum(c) << 4) + charNum(d);
}

unsigned int charInt(c, d, e, f)
unsigned char c;
unsigned char d;
unsigned char e;
unsigned char f;
{
    return (charByte(c,d) << 8) + charByte(e,f);
}

unsigned char upper(c)
unsigned char c;
{
    if(c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

void printsrc(sourcestr)
srcstr *sourcestr;
{
    int i;
    printf("rec start: %c\n", sourcestr->rec_strt);
    printf("data length: %d\n", sourcestr->data_len);
    printf("address: %x\n", sourcestr->address);
    printf("data type: %x\n", sourcestr->data_typ);
    printf("data: ");
    for (i = 0; i < sourcestr->data_len; i++)
    {
        printf("%02x", sourcestr->data[i]);
    }
    puts("");
    printf("checksum: %x\n", sourcestr->chksum);
}

void cpystruct(sourcestr, destnstr, srcpos, dstpos, dstdmax)
srcstr *sourcestr;
deststr *destnstr;
unsigned int *srcpos;
unsigned int *dstpos;
unsigned char dstdmax;
{
    int iter_len = sourcestr->data_len - *srcpos;

    if (iter_len > dstdmax)
    {
        iter_len = dstdmax;
    }

    destnstr->rec_strt = ';';

    if (*dstpos == 0)
        destnstr->address = sourcestr->address + *srcpos;

    memcpy(destnstr->data + *dstpos, sourcestr->data + *srcpos, iter_len);
    *srcpos += iter_len;
    *dstpos += iter_len;

    destnstr->data_len = *dstpos;
}

void ptpprint(destnstr, pfptr)
deststr *destnstr;
FILE* pfptr;
{
    int i;
    fprintf(pfptr, "%c", destnstr->rec_strt);
    fprintf(pfptr, "%02x", destnstr->data_len);
    fprintf(pfptr, "%04x", destnstr->address);
    for (i = 0; i < destnstr->data_len; i++)
    {
        fprintf(pfptr, "%02x", *(destnstr->data+i));
    }

    destnstr->chksum = destnstr->data_len + (destnstr->address >> 8) + (destnstr->address & 0xFF);

    for (i = 0; i < destnstr->data_len; i++)
        destnstr->chksum += *(destnstr->data + i);

    fprintf(pfptr, "%04x", destnstr->chksum);

    fprintf(pfptr, "%c", '\n');
}

int parseargs(argc, argv, MAXRC, model, fptr, pfptr)
int argc;
char *argv[];
unsigned char *MAXRC;
unsigned char *model;
FILE **fptr;
FILE **pfptr;
{
    if (argv[1][0] == '-')
    {
        switch(argv[1][1])
        {
            case 'k':
            case 'K': *model = 'k'; *MAXRC = 24;
                    break;
            case 's':
            case 'S': *model = 's'; *MAXRC = 16;
                    break;
        }
    }
    else
    {
        printf("No target board specified\n");
        return -3;
    }

    if (argv[2][0] == '-')
    {
        if (argv[2][1] == 'b' || argv[2][1] == 'B')
        {
            *MAXRC = atoi(argv[3]);
        }
        else
        {
            printf("Incorrect parameter\n");
            return -4;
        }
    }

    if ((*fptr = fopen(argv[argc - 2], "r")) == NULL)
    {
        printf("File does not exist\n");
        return -1;
    }

    if ((*pfptr = fopen(argv[argc - 1], "w")) == NULL)
    {
        printf("Syntax: mosconv -s/k [-b xx] test.asm test.ptp\n");
        return -2;
    }
    return 0;
}


#endif // MAIN_H

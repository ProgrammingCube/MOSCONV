/* Intel Hex Object Code to
 * MOS File Format Converter
 *
 * Patrick Jackson
 *
 * Converts Intel Hex Object File Format
 * into MOS File Format. Used for paper
 * tape and ROM programming.
 * 2/4/2021
 *
 */

#define CPM 0
#define AZTEC_C 0
#define HI_TECH 0

#include "stdio.h"
#if CPM == 0 && AZTEC_C == 0
#include "stdlib.h"
#include "string.h"
#endif /* CPM */

#define BUFFER  256

#if AZTEC_C == 1
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
int c;
int len;
{
    unsigned char *dst = s;
    while (len > 0)
    {
        *dst = (unsigned char) c;
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

int main(argc, argv)
int argc;
char *argv[];
{
    unsigned char line[BUFFER];
    unsigned char *dataline;
    unsigned char byteline[24];
    unsigned char headbyte[3];
    unsigned char srcllen;
    unsigned char datallen;
    int i;
    unsigned char numBytes;
    unsigned short checkSum = 0;
    unsigned char arraycnt = 0;
    unsigned char MAXRC;
    unsigned char model;
    unsigned char reccount = 0;
    FILE *fptr;
    FILE *pfptr;

    /* mosconv -s/k [-b xx] add.hex add.ptp */

    if (argv[1][0] == '-')
    {
        switch(argv[1][1])
        {
            case 'k': model = 'k'; MAXRC = 24; break;
            case 'K': model = 'k'; MAXRC = 24; break;
            case 's': model = 's'; MAXRC = 16; break;
            case 'S': model = 's'; MAXRC = 16; break;
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
            MAXRC = atoi(argv[3]);
        }
        else
        {
            printf("Incorrect parameter\n");
            return -4;
        }
    }

    if ((fptr = fopen(argv[argc - 2], "r")) == NULL)
    {
        printf("File does not exist\n");
        return -1;
    }

    if ((pfptr = fopen(argv[argc - 1], "w")) == NULL)
    {
        printf("Syntax: mosconv -s/k [-b xx] test.asm test.ptp\n");
        return -2;
    }

    memset(line, '\0', BUFFER);
    memset(byteline, '\0', 24);
    memset(headbyte, '\0', 3);

    printf("Intel Hex Object Code to\nMOS File Format Converter\n");
    printf("\nby Patrick Jackson, 2021\nLoading file...\n");

    while (fgets(line, BUFFER, fptr))
    {
        srcllen = strlen(line);

        if (srcllen > 12)
        {
            reccount += 1;
            datallen = srcllen - 12;
            dataline = (unsigned char *)malloc(datallen);
            datacopy(line, dataline, datallen);
            for ( i = 0; i < datallen; i+=2)
            {
                byteline[i/2] = charByte(dataline[i], dataline[i+1]);
                /* printf("%x\n",byteline[i/2]); */
            }
            headbyte[0] = charByte(line[1], line[2]);
            headbyte[1] = charByte(line[3], line[4]);
            headbyte[2] = charByte(line[5], line[6]);

            /* printf("datallen %d\n",datallen); */
            datallen = datallen / 2;  /* bytes size */
            while(datallen > 0)
            {
                /* printf("------------\n"); */

                numBytes = datallen % MAXRC;
                if(numBytes == 0) numBytes = MAXRC;
                for (i = 0; i < numBytes; i++)
                {
                    checkSum += byteline[arraycnt + i];
                }

                /* logic to write to a file */
                fprintf(pfptr, "%c", ';');
                fprintf(pfptr, "%02x", numBytes);
                fprintf(pfptr, "%02x", headbyte[1]);
                fprintf(pfptr, "%02x", headbyte[2]);
                for (i = 0; i < numBytes; i++)
                {
                    fprintf(pfptr, "%02x", byteline[arraycnt + i]);
                }
                fprintf(pfptr, "%04x", checkSum);
                if (model == 'k')
                {
                    fprintf(pfptr, "%c", 0x0D); /* cr */
                    fprintf(pfptr, "%c", 0x0A); /* lf */

                    fprintf(pfptr, "%c", 0x00); /* null */
                    fprintf(pfptr, "%c", 0x00); /* null */
                    fprintf(pfptr, "%c", 0x00); /* null */
                    fprintf(pfptr, "%c", 0x00); /* null */
                    fprintf(pfptr, "%c", 0x00); /* null */
                    fprintf(pfptr, "%c", 0x00); /* null */
                }
                if (model == 's')
                {
                    fprintf(pfptr, "%c", '\n');
                }

                datallen -= numBytes;
                arraycnt += numBytes;
                checkSum = 0;
                /* increment header */
                if(headbyte[2] + datallen > 255) headbyte[1] += numBytes;
                else headbyte[2] += numBytes;
            }
        }

        memset(line, '\0', BUFFER);
    }
    printf("Finishing file...\n");
    if (model == 'k')   /* print footer */
    {
        fprintf(pfptr, "%s", ";00");
        fprintf(pfptr, "%04x", reccount);
        fprintf(pfptr, "%04x", reccount);   /* hack checksum */
        fprintf(pfptr, "%c", 0x13);         /* XOFF */
    }
    if (model == 's')
    {
        fprintf(pfptr, "%s", ";00\r");
    }


    fclose(fptr);
    fclose(pfptr);
    free(dataline);
    return(0);
}


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

#define CPM 1
#define AZTEC_C 1
#define HI_TECH 0
#define MAXREC 32

#if CPM == 0 && AZTEC_C == 0
#include "stdlib.h"
#include "string.h"
#endif /* CPM */

#define BUFFER  256

#include "mosconv.h"

int main(argc, argv)
int argc;
char *argv[];
{
    srcstr sourcestr;
    deststr destnstr;
    int asciidlen;

    unsigned char line[BUFFER];
    unsigned char *dataline;
    unsigned char byteline[MAXREC];
    unsigned char headbyte[3];
    unsigned char srcllen;
    unsigned char datallen;
    int i;
    unsigned char numBytes;

    unsigned int dstpos;
    unsigned int srcpos;

    unsigned short checkSum = 0;
    unsigned char arraycnt = 0;
    unsigned char MAXRC;
    unsigned char model;
    unsigned short reccount = 0;
    FILE *fptr = 0;
    FILE *pfptr = 0;


    /* mosconv -s/k [-b xx] add.hex add.ptp [n]*/

    if (parseargs(argc, argv, &MAXRC, &model, &fptr, &pfptr) < 0)
        puts("exiting");

    memset(line, '\0', BUFFER);
    memset(byteline, '\0', MAXREC);
    memset(headbyte, '\0', 3);

    printf("Intel Hex Object Code to\nMOS File Format Converter\n");
    printf("\nby Patrick Jackson, 2021\nLoading file...\n");

    dstpos = 0;

    while (fgets(line, BUFFER, fptr))
    {
        srcllen = strlen(line);

        if (srcllen > 12)
        {
            /* fill struct */

            sourcestr.rec_strt = line[0];
            sourcestr.data_len = charByte(line[1], line[2]);
            sourcestr.address = charInt(line[3], line[4], line[5], line[6]);
            sourcestr.data_typ = charByte(line[7], line[8]);

            asciidlen = sourcestr.data_len << 1;
            for ( i = 0; i < asciidlen; i+=2)
                sourcestr.data[i/2] = charByte(line[i + 9], line[i+1 + 9]);

            sourcestr.chksum = charByte(line[9 + asciidlen], line[10 + asciidlen]);

            srcpos = 0;

            while (srcpos < sourcestr.data_len)
            {
                if ((dstpos > 0) && ((destnstr.address + dstpos) != srcpos))
                {
                    ptpprint(&destnstr, pfptr);
                    dstpos = 0;
                }

                cpystruct(&sourcestr, &destnstr, &srcpos, &dstpos, MAXRC);

                if (dstpos == MAXRC)
                {
                    ptpprint(&destnstr, pfptr);
                    dstpos = 0;
                }
            }
        }

        memset(line, '\0', BUFFER);
    }

    if (dstpos != 0)
    {
        ptpprint(&destnstr, pfptr);
    }

    printf("Finishing file...\n");
    if (model == 'k')   /* print footer */
    {
        fprintf(pfptr, "%s", ";00");
        fprintf(pfptr, "%04x", reccount);
        fprintf(pfptr, "%04x", (unsigned char)(reccount >> 8) + (unsigned char)(reccount & 0xFF));
        fprintf(pfptr, "%c", 0x13);         /* XOFF */
    }
    if (model == 's')
    {
        fprintf(pfptr, "%s", ";00");
        fprintf(pfptr, "%c", 0x0D);
    }


    fclose(fptr);
    fclose(pfptr);
    free(dataline);
    return(0);
}

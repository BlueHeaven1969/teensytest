/*
 * strconv.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
#include "ff.h"

#include "strconv.h"

int STRCONV__UTF16toUTF8(TCHAR *datain, uint8_t *dataout)
{
    int i, j=0;
    for (i=0;i<MAX_STRLEN;i++)
    {
        if (datain[i]==0) break;
        if (datain[i]<0x80)
        {
            dataout[j++]=datain[i];
        }
        else if (datain[i]<0x800)
        {
            dataout[j++]=0xC0 + (datain[i] >> 6);
            dataout[j++]=0x80 + (datain[i] & 0x3F);
        }
        else
        {
            dataout[j++]=0xE0 + (datain[i] >> 12);
            dataout[j++]=0x80 + ((datain[i] >> 6) & 0x3F);
            dataout[j++]=0x80 + (datain[i] & 0x3F);
        }
    }

    return j;
}



/*
 * EOF - file ends with blank line
 */

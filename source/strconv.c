/*
 * strconv.c
 *
 *  Created on: Jan 6, 2018
 *      Author: rsp
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ff.h"

#include "uart.h"
#include "strconv.h"
#include "player.h"

static int cmp(PlayerFileList_t *a, PlayerFileList_t *b);

int STRCONV__UTF16toUTF8(TCHAR *datain, uint8_t *dataout, UartColor_t color)
{
    int i, j=0;
    if (color)
    {
        j=sprintf((char *)dataout,"\x1b[%dm",color);
    }
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
    if (color)
    {
        dataout[j++] = 0x1B;
        dataout[j++] = '[';
        dataout[j++] = '0';
        dataout[j++] = 'm';
    }
    dataout[j]=0;

    return j;
}

void STRCONV__UTF16toLCD(TCHAR *datain, uint8_t *dataout, size_t bufflen)
{
    char msg[40];
    int i,j=0;
    memset(dataout,0,bufflen);

    for (i=0;i<MAX_STRLEN;i++)
    {
        if (datain[i]==0) break;
        if (datain[i] < 0x80)
        {
            // standard ascii
            dataout[j++] = datain[i];
        }
        else
        {
            switch (datain[i])
            {
                case 0xC0:
                case 0xC1:
                case 0xC2:
                case 0xC3:
                case 0xC4:
                case 0xC5:
                case 0xC6:
                case 0x100:
                case 0x102:
                case 0x104:
                    dataout[j++] = 'A';
                    break;
                case 0xC7:
                case 0x106:
                case 0x108:
                case 0x10A:
                    dataout[j++] = 'C';
                    break;
                case 0xD0:
                case 0x10E:
                case 0x110:
                    dataout[j++] = 'D';
                    break;
                case 0xC8:
                case 0xC9:
                case 0xCA:
                case 0xCB:
                case 0x112:
                case 0x114:
                case 0x116:
                case 0x118:
                case 0x11a:
                    dataout[j++] = 'E';
                    break;
                case 0x11C:
                case 0x11E:
                case 0x120:
                case 0x122:
                    dataout[j++] = 'G';
                    break;
                case 0x124:
                case 0x126:
                    dataout[j++] = 'H';
                    break;
                case 0xCC:
                case 0xCD:
                case 0xCE:
                case 0xCF:
                case 0x128:
                case 0x12a:
                case 0x12c:
                case 0x12e:
                case 0x130:
                case 0x132:
                    dataout[j++] = 'I';
                    break;
                case 0x134:
                    dataout[j++] = 'J';
                    break;
                case 0x136:
                    dataout[j++] = 'K';
                    break;
                case 0x139:
                case 0x13b:
                case 0x13d:
                case 0x13f:
                case 0x141:
                    dataout[j++] = 'L';
                    break;
                case 0xD1:
                case 0x143:
                case 0x145:
                case 0x147:
                case 0x14a:
                    dataout[j++] = 'N';
                    break;
                case 0xD2:
                case 0xD3:
                case 0xD4:
                case 0xD5:
                case 0xD6:
                case 0xD8:
                case 0x14C:
                case 0x14E:
                case 0x150:
                case 0x152:
                    dataout[j++] = 'O';
                    break;
                case 0x154:
                case 0x156:
                case 0x158:
                    dataout[j++] = 'R';
                    break;
                case 0x15a:
                case 0x15c:
                case 0x15e:
                case 0x160:
                    dataout[j++] = 'S';
                    break;
                case 0x162:
                case 0x164:
                case 0x166:
                    dataout[j++] = 'T';
                    break;
                case 0xD9:
                case 0xDA:
                case 0xDB:
                case 0xDC:
                case 0x168:
                case 0x16a:
                case 0x16c:
                case 0x16e:
                case 0x170:
                case 0x172:
                    dataout[j++] = 'U';
                    break;
                case 0x174:
                    dataout[j++] = 'W';
                    break;
                case 0xDD:
                case 0x176:
                case 0x178:
                    dataout[j++] = 'Y';
                    break;
                case 0x179:
                case 0x17b:
                case 0x17d:
                    dataout[j++] = 'Z';
                    break;
                case 0xE0:
                case 0xE1:
                case 0xE2:
                case 0xE3:
                case 0xE4:
                case 0xE5:
                case 0xE6:
                case 0x101:
                case 0x103:
                case 0x105:
                    dataout[j++] = 'a';
                    break;
                case 0xE7:
                case 0x107:
                case 0x109:
                case 0x10B:
                    dataout[j++] = 'c';
                    break;
                case 0x10F:
                case 0x111:
                    dataout[j++] = 'd';
                    break;
                case 0xE8:
                case 0xE9:
                case 0xEA:
                case 0xEB:
                case 0x113:
                case 0x115:
                case 0x117:
                case 0x119:
                case 0x11b:
                    dataout[j++] = 'e';
                    break;
                case 0x11d:
                case 0x11f:
                case 0x121:
                case 0x123:
                    dataout[j++] = 'g';
                    break;
                case 0x125:
                case 0x127:
                    dataout[j++] = 'h';
                    break;
                case 0xEC:
                case 0xED:
                case 0xee:
                case 0xef:
                case 0x129:
                case 0x12b:
                case 0x12d:
                case 0x12f:
                case 0x131:
                case 0x133:
                    dataout[j++] = 'i';
                    break;
                case 0x135:
                    dataout[j++] = 'j';
                    break;
                case 0x137:
                case 0x138:
                    dataout[j++] = 'k';
                    break;
                case 0x13a:
                case 0x13c:
                case 0x13e:
                case 0x140:
                case 0x142:
                    dataout[j++] = 'l';
                    break;
                case 0xf1:
                case 0x144:
                case 0x146:
                case 0x148:
                case 0x149:
                case 0x14b:
                    dataout[j++] = 'n';
                    break;
                case 0xf0:
                case 0xf2:
                case 0xf3:
                case 0xf4:
                case 0xf5:
                case 0xf6:
                case 0xf8:
                case 0x14d:
                case 0x14f:
                case 0x151:
                case 0x153:
                    dataout[j++] = 'o';
                    break;
                case 0x155:
                case 0x157:
                case 0x159:
                    dataout[j++] = 'r';
                    break;
                case 0x15b:
                case 0x15d:
                case 0x15f:
                case 0x161:
                    dataout[j++] = 's';
                    break;
                case 0x163:
                case 0x165:
                case 0x167:
                    dataout[j++] = 't';
                    break;
                case 0xf9:
                case 0xfa:
                case 0xfb:
                case 0xfc:
                case 0x169:
                case 0x16b:
                case 0x16d:
                case 0x16f:
                case 0x171:
                case 0x173:
                    dataout[j++] = 'u';
                    break;
                case 0x175:
                    dataout[j++] = 'w';
                    break;
                case 0xfd:
                case 0xff:
                case 0x177:
                    dataout[j++] = 'y';
                    break;
                case 0x17a:
                case 0x17c:
                case 0x17e:
                    dataout[j++] = 'z';
                    break;
                case 0x2010:
                case 0x2011:
                case 0x2012:
                case 0x2013:
                case 0x2014:
                case 0x2015:
                    dataout[j++] = '-';
                    break;
                case 0x2018:
                case 0x2019:
                    dataout[j++] = '\'';
                    break;
                case 0x2026:
                    dataout[j++] = '.';
                    break;
                case 0x221E:
                    dataout[j++] = '8';
                    break;
                case 0x2605:
                    dataout[j++] = '*';
                    break;
                case 0x266F:
                    dataout[j++] = '#';
                    break;
                default:
                    dataout[j++] = ' ';
                    sprintf(msg,"Unhandled Unicode!! -- %04x\r\n",datain[i]);
                    UART__SendASCII(msg, UART_COLOR_MAGENTA);
            }
        }
        if (j >= bufflen) break;
    }
}
/*
 * This is the actual sort function. Notice that it returns the new
 * head of the list. (It has to, because the head will not
 * generally be the same element after the sort.) So unlike sorting
 * an array, where you can do
 *
 *     sort(myarray);
 *
 * you now have to do
 *
 *     list = listsort(mylist);
 */
PlayerFileList_t *STRCONV__listsort(PlayerFileList_t *list)
{
    PlayerFileList_t *p, *q, *e, *tail, *oldhead;
    int insize, nmerges, psize, qsize, i;

    /*
     * Silly special case: if `list' was passed in as NULL, return
     * NULL immediately.
     */
    if (!list) return NULL;

    insize = 1;

    while (1)
    {
        p = list;
        oldhead = list;            /* only used for circular linkage */
        list = NULL;
        tail = NULL;

        nmerges = 0;  /* count number of merges we do in this pass */

        while (p)
        {
            nmerges++;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++)
            {
                psize++;
                q = (q->next == oldhead ? NULL : q->next);
                if (!q) break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q))
            {
                /* decide whether next element of merge comes from p or q */
                if (psize == 0)
                {
                    /* p is empty; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                    if (q == oldhead) q = NULL;
                }
                else if (qsize == 0 || !q)
                {
                    /* q is empty; e must come from p. */
                    e = p;
                    p = p->next;
                    psize--;
                    if (p == oldhead) p = NULL;
                }
                else if (cmp(p,q) <= 0)
                {
                    /* First element of p is lower (or same);
                     * e must come from p. */
                    e = p;
                    p = p->next;
                    psize--;
                    if (p == oldhead) p = NULL;
                }
                else
                {
                    /* First element of q is lower; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                    if (q == oldhead) q = NULL;
                }

                /* add the next element to the merged list */
                if (tail)
                {
                    tail->next = e;
                }
                else
                {
                    list = e;
                }

                /* Maintain reverse pointers in a doubly linked list. */
                e->prev = tail;
                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        tail->next = list;
        list->prev = tail;

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
            return list;

        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }
}

static int cmp(PlayerFileList_t *a, PlayerFileList_t *b)
{
    char a_low[PLAYER_CHAR_WIDTH];
    char b_low[PLAYER_CHAR_WIDTH];
    int i;
    for (i=0;i<PLAYER_CHAR_WIDTH;i++)
    {
        a_low[i] = tolower(a->name[i]);
        b_low[i] = tolower(b->name[i]);
    }
    return strcmp(a_low, b_low);
}

/*
 * EOF - file ends with blank line
 */

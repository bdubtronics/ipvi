/*
   Copyright (C) 2011, Josmar Pierri

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MSG_PTBR
//#define MSG_EN
#include "ipvi-msg.h"


void line_centr_text(char *msg, char chr)
{
  int n, i;

  if(msg)
    n=(77-strlen(msg))/2;
  else
    n=79;
  for( i=0; i<n; ++i) putc(chr, stdout);
  if (msg)
  {
    printf(" %s ", msg );
    for( i=0; i<n; ++i) putc(chr, stdout);
  }
}

void write_banner()
{
  printf("\n");
  line_centr_text( NULL, '=');
  printf("\n");
  line_centr_text(MSG_BANNER, ' ');
  printf("\n");
  line_centr_text( NULL, '=');
  printf("\n");
}

void usage(char *s, char *e)
{
  char temp[200];
  
  if (e) printf("***> %s\n\n", e);
  else
  {
    write_banner();
    snprintf(temp, 199, "(C) 2011, Josmar Pierri  (%s GPLv3)", MSG_LICENSE);
    line_centr_text(temp, ' ');
    printf("\n\n%s:\n  %s <%s>\n", MSG_USAGE, s, MSG_PREFIX );
    printf("  (%s %s 2001:db8:123::1:2)\n\n", MSG_EXAMPLE, s );
    printf("  %s <%s>/<%s>\n", s, MSG_PREFIX, MSG_LENGTH );
    printf("  (%s %s 2001:db8:123::1:2/50)\n", MSG_EXAMPLE, s );
    printf("\n");
  }
}

char txtpref[200], txtlen[20];
int preflen;

char leftpref[200], rightpref[200];

int splitpref()
{
  int x=0, y=0, c;
  char *s;
  
  leftpref[x] = rightpref[y] = 0;
  s = txtpref;
  while ( (c=*s++) )
  {
    if ( (c==':') && (*s==':') ) { ++s; c=*s++; break; }
    leftpref[x++] = c;
    leftpref[x] = 0;
  }
  //
  while (c)
  {
    if (c!=':')
    {
      rightpref[y++] = c;
      rightpref[y] = 0;
    }
    else
    {
      if (*s==':') return 0;
      rightpref[y++] = c;
      rightpref[y] = 0;
    }
    c = *s++;
  }
  return 1;
}


void getlen(char *s)
{
  int i=0, c;
  
  txtlen[0] = 0;
  while ( (c=*s++) && (i < 5) )
  {
    txtlen[i++] = c;
    txtlen[i] = 0;
  }
  preflen=atoi(txtlen);
  if ( (preflen == 0) || (preflen<0) || (preflen>128) ) preflen=128;
}

int getprefix(char *s)
{
  int i=0, c;
  
  txtpref[0] = 0;
  while ( (c=*s++) && (i < 199) )
  {
    if ( c == '/' )  break;
    if ( ((c>='0') && (c<='9')) || ((c>='a') && (c<='f')) || ((c>='A') && (c<='F')) || (c==':') )
    {
      txtpref[i++] = c;
      txtpref[i] = 0;
    }
    else
      return 0;
  }
  if (c == '/') getlen(s); else preflen=128;
  return i;
}

unsigned int lw[8], rw[8], w[8], t[8], q[8];
int nl, nr;


int convert_left_right()
{
  int i;
  
  nl = sscanf( leftpref, "%4x:%4x:%4x:%4x:%4x:%4x:%4x:%4x", &lw[0], &lw[1], &lw[2], &lw[3], &lw[4], &lw[5], &lw[6], &lw[7] );
  nr = sscanf( rightpref, "%4x:%4x:%4x:%4x:%4x:%4x:%4x:%4x", &rw[0], &rw[1], &rw[2], &rw[3], &rw[4], &rw[5], &rw[6], &rw[7] );

  if ( (nl+nr) > 8 ) return 0;

  for( i=0; i<8; ++i )
    w[i] = 0;
  //
  for( i=0; i<nl; ++i )
    w[i] = lw[i];
  for( i=0; i<nr; ++i )
    w[7-i] = rw[nr-i-1];

  return 1;
}


char tpref[150], tmask[150], tw[150], tt[150], tq[150];

void bin2t(unsigned int v, char *s)
{
  int i, n=0x8000;
  
  i = strlen(s);
  while(n && (i<149))
  {
    s[i++] = (v & n) ? '1' : 'o';
    s[i] = 0;
    n >>= 1;
  }
}

void convb2t(unsigned int *w, char *t)
{
  int i;
  
  t[0] = 0;
  for( i=0; i<8; ++i )
    bin2t( w[i], t );
}  

unsigned int t2bin(char *s)
{
  unsigned int n=0x8000, v=0;
  
  while( n )
  {
    v |= (*s++ == '1') ? n : 0;
    n >>= 1;
  }
  return v;
}

void convt2b(char *t, unsigned int *w)
{
  int i;
  
  for( i=0; i<8; ++i )
  {
    w[i] = t2bin(t+(i*16));
  }
}

void genmask(int n, char *m)
{
  int i;
  
  for( i=0; i<128; ++i )
  {
    m[i] = (i < n) ? '1' : 'o';
  }
  m[i] = 0;
}

void logic_and(char *a, char *b, char *c)
{
  int i;
  
  for( i=0; i<128; ++i )
  {
    c[i] = ((a[i] == '1') && (b[i] == '1')) ? '1' : 'o';
  }
  c[i] = 0;
}
  
void logic_xor(char *a, char *b, char *c)
{
  int i;
  
  for( i=0; i<128; ++i )
  {
    c[i] = (a[i] == b[i]) ? 'o' : '1';
  }
  c[i] = 0;
}
  
void logic_or(char *a, char *b, char *c)
{
  int i;
  
  for( i=0; i<128; ++i )
  {
    c[i] = ((a[i] == '1') || (b[i] == '1')) ? '1' : 'o';
  }
  c[i] = 0;
}

void write_8w( unsigned int *d, char *lbl )
{
  int i;

  printf("%18s  -->  %04X : ", lbl, d[0]);
  for( i=1; i<7; ++i )
    printf("%04X : ", d[i] );
  printf("%04X\n", d[7]);
}

void write_8w_compr( unsigned int *d, char *lbl )
{
  int i, x, y, t, mt, mx, my;

  t=0; mt=-1;
  x=mx=0; y=my=0;
  for( i=7; i>=0; --i )
  {
    if (t==0)
    {
      if (d[i] == 0)
      {
        ++t; x=i; y=i;
      }
    }
    else
    {
      if (d[i] != 0)
      {
        if (t>mt)
        {
          mx=x; my=y; mt=t;
        }
        t=0;
      }
      else
      {
        ++t;
        x=i;
      }
    }
  }
  if (t>mt)
  {
    mx=x; my=y; mt=t;
  }

  printf("%18s  -->  ", lbl );
  if (mt>0)
  {
    for( i=0; i<mx; ++i )
    {
      if ( i>0 ) printf(":");
      printf("%x", d[i]);
    }
    printf("::");
    for( i=(my+1); i<8; ++i )
    {
      if ( i>(my+1) ) printf(":");
      printf("%x", d[i]);
    }
  }
  else
  {
    for( i=0; i<8; ++i )
    {
      if ( i>0 ) printf(":");
      printf("%x", d[i]);
    }
  }
}

char *ident_type( unsigned int *d )
{
  char *tipo = MSG_UNKNOWN_TYPE;
  
  if ( (d[0] & 0xe000 ) == 0x2000 )	tipo="Global Unicast (RFC 4291)";
  if ( d[0] == 0xfe80 ) 		tipo="Link-Local (RFC 4291)";
  if ( (d[0] & 0xfe00 ) == 0xfc00 )	tipo="ULA - Unique Local (RFC 4193)";
  if ( (d[0] & 0xff00 ) == 0xff00 )	tipo="Multicast (RFC 4291)";

  if ( (d[0] == 0x2001) && (d[1] == 0x0db8) )	tipo="Documentation (RFC 3849)";
  if ( (d[0] == 0x2001) && (d[1] == 0) )	tipo="Teredo (RFC 4380)";
  if ( d[0] == 0x2002 )				tipo="6to4 (RFC 3056)";

  if ( (d[0] == 0) && (d[1] == 0) && (d[2] == 0) && (d[3] == 0) &&
       (d[4] == 0) && (d[5] == 0) && (d[6] == 0) && (d[7] == 1) )
         tipo="Loopback (RFC 4291)";
  
  if ( (d[0] == 0) && (d[1] == 0) && (d[2] == 0) && (d[3] == 0) &&
       (d[4] == 0) && (d[5] == 0) && (d[6] == 0) && (d[7] == 0) )
         tipo="Unspecified (RFC 4291)";
  
  return tipo;
}

void write_area_zoom(char *s, int len)
{
  int i, ini, fim;
  
  ini = len-25;
  if (ini<0) ini=0;
  fim = len+25;
  if (fim>127) fim=127;
  
  if (ini>0) printf("..."); else printf("   ");
  for( i=ini; i<=fim; ++i )
  {
    if ( ((i & 15) == 0) && (i>0) && (i<127) ) printf(" : ");
    else
      if ( ((i & 3) == 0) && (i>0) ) printf(" ");
    printf("%c", s[i]);
  }
  if (fim<127) printf("...\n"); else printf("\n");
}  

void write_ptr_zoom(char *s, int len)
{
  int i, ini, fim;
  
  ini = len-25;
  if (ini<0) ini=0;
  fim = len+25;
  if (fim>127) fim=127;
  
  printf("   ");
  for( i=ini; i<len; ++i )
  {
    if ( ((i & 15) == 0) && (i>0) && (i<127) ) printf("   ");
    else
      if ( ((i & 3) == 0) && (i>0) ) printf(" ");
    printf("%c", (i == (len-1)) ? '|' : ' ');
  }
  printf("\n%3d", len);
  for( i=ini; i<len; ++i )
  {
    if ( ((i & 15) == 0) && (i>0) && (i<127) ) printf("---");
    else
      if ( ((i & 3) == 0) && (i>0) ) printf("-");
    printf("%c", (i == (len-1)) ? '+' : '-');
  }
  if (len<127)
    printf("    (%d %s)", 128-len, MSG_HOST_BITS);
  else
    if (len==127)
      printf("    (1 %s)", MSG_HOST_BIT);
  printf("\n");
}  


// Main
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    usage( argv[0], NULL );
    exit(0);
  }

  if (getprefix(argv[1]) )
  {

    if (splitpref())
    {
      if ( convert_left_right() )
      {
        write_banner();
        printf("\n%18s  -->  %s\n", MSG_TYPE, ident_type(w) );
        write_8w_compr( w, MSG_COMPRESSED );
        printf("/%d\n", preflen);
        write_8w( w, MSG_EXPANDED );

        if (preflen != 128)
        {
          convb2t( w, tpref);
          genmask(preflen, tmask);
        
          genmask(128, tw);
          logic_xor(tmask, tw, tt);
        
          convt2b(tt, t);
        
          logic_and(tpref, tmask, tw);
          convt2b(tw, w);
          write_8w( w, MSG_BLOCK_BEGIN );
        
          logic_or(tw, tt, tq);
          convt2b(tq, q);
          write_8w( q, MSG_BLOCK_END );
        
          printf("\n");
          line_centr_text(MSG_ZOOM_BLK_BIN, '~');
          printf("\n\n");
          write_area_zoom(tw,preflen);
          write_area_zoom(tq,preflen);
          write_ptr_zoom(tq,preflen);
        }
        printf("\n");
      }
      else
        usage(argv[0], MSG_ERR_TOO_MANY_WORDS );
    }
    else
     usage(argv[0], MSG_ERR_WRONG_USE );
    
  }
  else
    usage(argv[0], MSG_ERR_INV_CHR );

  return 0;
}

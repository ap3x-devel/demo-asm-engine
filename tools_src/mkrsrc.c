/* mkrsrc.c: builds src/res/icon.o (i386 COFF object with a .rsrc section: the exe icon)
   from src/res/icon_1..7.bin and logo.bin.  usage: mkrsrc res_dir     (plain C, coder: Ivandoe) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 6
#define NT 7
static unsigned char buf[1<<20]; static int len = 0;
static int relo[64], nrel = 0;
static void w32(int o, unsigned v){ memcpy(buf + o, &v, 4); }
static void w16(int o, unsigned v){ memcpy(buf + o, &v, 2); }
static int dir(int nid){ int o = len; len += 16 + 8*nid; w16(o + 14, nid); return o; }   /* directory with nid id entries */
static int ent(int d, int k, unsigned id, unsigned off){ w32(d + 16 + k*8, id); w32(d + 16 + k*8 + 4, off); return 0; }
int main(int argc, char **argv){
  const char *dr = argc > 1 ? argv[1] : ".";
  static unsigned char *img[NT]; int isz[NT]; static int sz[NT] = {16, 32, 48, 64, 128, 0, 32};
  for(int i = 0; i < NT; i++){ char nm[256]; snprintf(nm, sizeof nm, "%s/icon_%d.bin", dr, i + 1);
    FILE *f = fopen(nm, "rb"); if(!f){ perror(nm); return 1; } img[i] = malloc(1<<19); isz[i] = fread(img[i], 1, 1<<19, f); fclose(f); }
  int root = dir(3), dB = dir(1), dI = dir(NT), dG = dir(2);
  ent(root, 0, 2, 0x80000000u | dB); ent(root, 1, 3, 0x80000000u | dI); ent(root, 2, 14, 0x80000000u | dG);
  int dbn = dir(1); ent(dB, 0, 1, 0x80000000u | dbn);
  int ebm = len; len += 16; ent(dbn, 0, 0x409, ebm);
  int dn[NT], de[NT], dd[NT];
  for(int i = 0; i < NT; i++){ dn[i] = dir(1); ent(dI, i, i + 1, 0x80000000u | dn[i]); }
  int dg = dir(1), dg2 = dir(1); ent(dG, 0, 1, 0x80000000u | dg); ent(dG, 1, 2, 0x80000000u | dg2);
  for(int i = 0; i < NT; i++){ de[i] = len; len += 16; ent(dn[i], 0, 0x409, de[i]); }
  int eg = len; len += 16; ent(dg, 0, 0x409, eg); int eg2 = len; len += 16; ent(dg2, 0, 0x409, eg2);
  /* icon group */
  static unsigned char logo[1<<20]; int lsz; { char nm2[256]; snprintf(nm2, sizeof nm2, "%s/logo.bin", dr); FILE *fl = fopen(nm2, "rb"); if(!fl){ perror(nm2); return 1; } lsz = fread(logo, 1, sizeof logo, fl); fclose(fl); }
  int grp = len; w16(grp, 0); w16(grp + 2, 1); w16(grp + 4, N); len += 6;
  for(int i = 0; i < N; i++){ int o = len; buf[o] = sz[i]; buf[o+1] = sz[i]; w16(o + 4, 1); w16(o + 6, 32); w32(o + 8, isz[i]); w16(o + 12, i + 1); len += 14; }
  int grpe = len; len = (len + 3) & ~3;
  for(int i = 0; i < NT; i++){ dd[i] = len; memcpy(buf + len, img[i], isz[i]); len += (isz[i] + 3) & ~3; }
  int ld = len; memcpy(buf + len, logo, lsz); len += (lsz + 3) & ~3;
  w32(ebm, ld); relo[nrel++] = ebm; w32(ebm + 4, lsz);
  for(int i = 0; i < NT; i++){ w32(de[i], dd[i]); relo[nrel++] = de[i]; w32(de[i] + 4, isz[i]); }
  w32(eg, grp); relo[nrel++] = eg; w32(eg + 4, grpe - grp);
  int grp2 = len; w16(grp2, 0); w16(grp2 + 2, 1); w16(grp2 + 4, 1); len += 6;   /* group 2: icon of the export button */
  { int o = len; buf[o] = 32; buf[o+1] = 32; w16(o + 4, 1); w16(o + 6, 32); w32(o + 8, isz[6]); w16(o + 12, 7); len += 14; }
  int grp2e = len; len = (len + 3) & ~3;
  w32(eg2, grp2); relo[nrel++] = eg2; w32(eg2 + 4, grp2e - grp2);
  /* COFF object */
  unsigned char hd[20 + 40]; memset(hd, 0, sizeof hd);
  int praw = 60, prel = praw + len, psym = prel + nrel * 10;
  w16(0, 0) ; hd[0] = 0x4c; hd[1] = 0x01; hd[2] = 1;                     /* machine i386, 1 section */
  memcpy(hd + 8, &psym, 4); { unsigned ns = 1; memcpy(hd + 12, &ns, 4); } hd[18] = 4; hd[19] = 1;
  memcpy(hd + 20, ".rsrc", 5);
  { unsigned v = len; memcpy(hd + 20 + 16, &v, 4); v = praw; memcpy(hd + 20 + 20, &v, 4); v = prel; memcpy(hd + 20 + 24, &v, 4);
    unsigned short nr = nrel; memcpy(hd + 20 + 32, &nr, 2); v = 0x40300040u; memcpy(hd + 20 + 36, &v, 4); }
  char nm[256]; snprintf(nm, sizeof nm, "%s/icon.o", dr);
  FILE *fo = fopen(nm, "wb"); fwrite(hd, 1, 60, fo); fwrite(buf, 1, len, fo);
  for(int i = 0; i < nrel; i++){ unsigned a = relo[i], s = 0; unsigned short t = 7; fwrite(&a, 4, 1, fo); fwrite(&s, 4, 1, fo); fwrite(&t, 2, 1, fo); }
  unsigned char sym[18]; memset(sym, 0, 18); memcpy(sym, ".rsrc", 5); sym[12] = 1; sym[14] = 0; sym[16] = 3;   /* section 1, STATIC */
  fwrite(sym, 1, 18, fo); unsigned strl = 4; fwrite(&strl, 4, 1, fo); fclose(fo);
  return 0;
}

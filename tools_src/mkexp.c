/* mkexp.c: draws the vector icon of the export button (film clapper + down arrow)
   -> src/res/icon_7.bin (32-bit DIB, 32x32 with alpha) and src/res/export.svg (same drawing as SVG).
   usage: mkexp res_dir      (plain C, coder: Ivandoe) */
#include <stdio.h>
#include <math.h>
#include <string.h>
#define S 32
#define SS 8                                   /* supersampling */
static float sdbox(float x, float y, float cx, float cy, float hx, float hy, float r){
  float dx = fabsf(x - cx) - (hx - r), dy = fabsf(y - cy) - (hy - r);
  return sqrtf(fmaxf(dx,0)*fmaxf(dx,0) + fmaxf(dy,0)*fmaxf(dy,0)) + fminf(fmaxf(dx,dy),0) - r;
}
/* coordinates in a 32x32 frame; returns the RGBA color (0..1) of the point */
static void shade(float x, float y, float *c){
  c[0]=c[1]=c[2]=c[3]=0;
  /* film strip: dark rounded rectangle with orange border, perforations */
  float d = sdbox(x, y, 16, 12, 14, 9.5f, 2.5f);
  if(d < 0){
    c[0]=0.12f; c[1]=0.13f; c[2]=0.17f; c[3]=1;
    if(d > -1.4f){ c[0]=1.0f; c[1]=0.62f; c[2]=0.15f; }
    for(int k = 0; k < 5; k++){                 /* perforations top and bottom */
      float px = 5.0f + k * 5.5f;
      if(sdbox(x, y, px, 5.2f, 1.3f, 1.0f, 0.4f) < 0 || sdbox(x, y, px, 18.8f, 1.3f, 1.0f, 0.4f) < 0){ c[0]=1; c[1]=0.85f; c[2]=0.55f; }
    }
    if(sdbox(x, y, 16, 12, 8.5f, 4.0f, 1.0f) < 0){ c[0]=0.95f; c[1]=0.96f; c[2]=1.0f; }   /* frame */
  }
  /* down arrow (body + tip) in light green */
  float ax = x - 16;
  int in = (fabsf(ax) < 2.3f && y > 20.5f && y < 26.0f) ||
           (y >= 25.0f && y < 30.5f && fabsf(ax) < (30.5f - y) * 1.15f + 0.01f);
  if(in){ c[0]=0.35f; c[1]=0.95f; c[2]=0.55f; c[3]=1; }
}
int main(int argc, char **argv){
  const char *dr = argc > 1 ? argv[1] : ".";
  char nm[256]; snprintf(nm, sizeof nm, "%s/icon_7.bin", dr);
  FILE *fo = fopen(nm, "wb"); if(!fo){ perror(nm); return 1; }
  unsigned hdr[10] = {40, S, S*2, 0, 0, S*S*4 + S*4, 0, 0, 0, 0}; unsigned short pl = 1, bpp = 32;
  fwrite(&hdr[0], 4, 3, fo); fwrite(&pl, 2, 1, fo); fwrite(&bpp, 2, 1, fo); fwrite(&hdr[4], 4, 6, fo);
  for(int y = S-1; y >= 0; y--) for(int x = 0; x < S; x++){
    float acc[4] = {0,0,0,0};
    for(int j = 0; j < SS; j++) for(int i = 0; i < SS; i++){
      float c[4]; shade(x + (i + 0.5f)/SS, y + (j + 0.5f)/SS, c);
      acc[0] += c[0]*c[3]; acc[1] += c[1]*c[3]; acc[2] += c[2]*c[3]; acc[3] += c[3];
    }
    float a = acc[3] / (SS*SS), k = acc[3] > 0 ? 1.0f / acc[3] : 0;
    unsigned char px[4] = { (unsigned char)(255*acc[2]*k), (unsigned char)(255*acc[1]*k), (unsigned char)(255*acc[0]*k), (unsigned char)(255*a) };
    fwrite(px, 1, 4, fo);
  }
  unsigned char z[S*4]; memset(z, 0, sizeof z); fwrite(z, 1, sizeof z, fo); fclose(fo);
  snprintf(nm, sizeof nm, "%s/export.svg", dr); FILE *fs = fopen(nm, "w"); if(!fs) return 1;
  fputs("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"32\" height=\"32\" viewBox=\"0 0 32 32\">\n"
        " <rect x=\"2\" y=\"2.5\" width=\"28\" height=\"19\" rx=\"2.5\" fill=\"#1f212b\" stroke=\"#ff9e26\" stroke-width=\"1.4\"/>\n"
        " <rect x=\"7.5\" y=\"8\" width=\"17\" height=\"8\" rx=\"1\" fill=\"#f2f5ff\"/>\n", fs);
  for(int k = 0; k < 5; k++) fprintf(fs, " <rect x=\"%.1f\" y=\"4.2\" width=\"2.6\" height=\"2\" rx=\"0.4\" fill=\"#ffd88c\"/><rect x=\"%.1f\" y=\"17.8\" width=\"2.6\" height=\"2\" rx=\"0.4\" fill=\"#ffd88c\"/>\n", 3.7f + k*5.5f, 3.7f + k*5.5f);
  fputs(" <path d=\"M13.7 20.5h4.6V25h3.7L16 30.5 10 25h3.7z\" fill=\"#59f28c\"/>\n</svg>\n", fs); fclose(fs);
  return 0;
}

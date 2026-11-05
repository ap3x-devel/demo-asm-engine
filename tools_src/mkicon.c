/* mkicon.c: generates the exe icon images (32-bit DIB) -> src/res/icon_N.bin
   usage: mkicon output_dir      (plain C, coder: Ivandoe)
   Drawing: particle trefoil knot in gold/white/cyan, particle ground, lava fountains. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define R 256
static float img[R][R][3];

static unsigned rs = 12345u;
static float rnd(void){ rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return (rs >> 8) * (1.0f / 16777216.0f); }

static void dot(float x, float y, float s, float cr, float cg, float cb, float a){
  int x0 = (int)floorf(x - 3*s), x1 = (int)ceilf(x + 3*s), y0 = (int)floorf(y - 3*s), y1 = (int)ceilf(y + 3*s);
  for(int j = y0; j <= y1; j++) for(int i = x0; i <= x1; i++){
    if(i < 0 || j < 0 || i >= R || j >= R) continue;
    float dx = i + 0.5f - x, dy = j + 0.5f - y;
    float w = expf(-(dx*dx + dy*dy) / (2*s*s)) * a;
    img[j][i][0] += cr*w; img[j][i][1] += cg*w; img[j][i][2] += cb*w;
  }
}
static void blur(float k){
  static float tmp[R][R][3];
  for(int pass = 0; pass < 2; pass++)
    for(int c = 0; c < 3; c++){
      for(int j = 0; j < R; j++) for(int i = 0; i < R; i++){
        float s = 0; int n = 0;
        for(int d = -6; d <= 6; d++){ int ii = i + d; if(ii >= 0 && ii < R){ s += img[j][ii][c]; n++; } }
        tmp[j][i][c] = s / n;
      }
      for(int j = 0; j < R; j++) for(int i = 0; i < R; i++){
        float s = 0; int n = 0;
        for(int d = -6; d <= 6; d++){ int jj = j + d; if(jj >= 0 && jj < R){ s += tmp[jj][i][c]; n++; } }
        img[j][i][c] += k * (s / n) * (pass ? 0.5f : 1.0f);
      }
    }
}

int main(int argc, char **argv){
  const char *dir = argc > 1 ? argv[1] : ".";
  /* background: night-blue gradient + central halo */
  for(int j = 0; j < R; j++) for(int i = 0; i < R; i++){
    float v = j / (float)R, dx = (i - 128) / 128.0f, dy = (j - 118) / 128.0f;
    float h = expf(-(dx*dx + dy*dy) * 2.2f);
    img[j][i][0] = 0.015f + 0.05f * h + 0.02f * v;
    img[j][i][1] = 0.025f + 0.07f * h + 0.015f * v;
    img[j][i][2] = 0.06f + 0.16f * h;
  }
  /* ground: particles in perspective */
  for(int n = 0; n < 5000; n++){
    float d = rnd(); d = d * d;                       /* near = rare towards the top */
    float y = 176 + 80 * powf(rnd(), 0.6f);
    float x = 128 + (rnd() - 0.5f) * (260 + 120 * (y - 176) / 80);
    float f = 0.15f + 0.3f * (y - 176) / 80;
    dot(x, y, 0.8f, 0.9f * f, 0.65f * f, 0.35f * f, 0.8f); (void)d;
  }
  /* trefoil knot made of particles */
  for(int n = 0; n < 9000; n++){
    float s = rnd() * 6.2832f;
    float px = sinf(s) + 2*sinf(2*s), py = cosf(s) - 2*cosf(2*s), pz = -sinf(3*s);
    float tr = 0.22f, ta = rnd() * 6.2832f, tb = sqrtf(rnd()) * tr;
    px += tb * cosf(ta); py += tb * sinf(ta); pz += tb * cosf(ta * 1.7f);
    float ay = 0.9f, ax = 0.7f;                      /* fixed rotation */
    float x1 = px * cosf(ay) + pz * sinf(ay), z1 = -px * sinf(ay) + pz * cosf(ay);
    float y1 = py * cosf(ax) - z1 * sinf(ax), z2 = py * sinf(ax) + z1 * cosf(ax);
    float sc = 34.0f / (1.0f - z2 * 0.05f);
    float X = 128 + x1 * sc, Y = 108 + y1 * sc;
    float t = 0.5f + 0.5f * sinf(s * 1.0f + 1.0f);
    float cr = 1.0f - 0.55f * t, cg = 0.82f + 0.12f * t, cb = 0.45f + 0.55f * t;
    float br = 0.75f + 0.25f * (z2 / 3.0f);
    dot(X, Y, 1.05f, cr, cg, cb, 0.55f * br);
  }
  /* lava fountains */
  for(int k = 0; k < 2; k++){
    float bx = k ? 205.0f : 52.0f, by = 214.0f;
    for(int n = 0; n < 260; n++){
      float tt = rnd(), a = (rnd() - 0.5f) * 1.6f + (k ? -0.5f : 0.5f), v = 55 + 40 * rnd();
      float x = bx + sinf(a) * v * tt * 1.1f, y = by - cosf(a) * v * tt * 1.3f + 70 * tt * tt;
      float f = 1.0f - 0.5f * tt;
      dot(x, y, 0.95f, 1.0f * f, 0.45f * f, 0.1f * f, 0.9f);
    }
  }
  blur(0.55f);
  /* final 256x256 image: tonemap + rounded-square mask */
  static unsigned char out[R][R][4];
  for(int j = 0; j < R; j++) for(int i = 0; i < R; i++){
    for(int c = 0; c < 3; c++){ float v = 1.0f - expf(-img[j][i][c] * 1.5f); out[j][i][c] = (unsigned char)(255 * powf(v, 0.85f)); }
    float cx = fabsf(i + 0.5f - 128) - (128 - 44), cy = fabsf(j + 0.5f - 128) - (128 - 44);
    float d = sqrtf(fmaxf(cx, 0)*fmaxf(cx, 0) + fmaxf(cy, 0)*fmaxf(cy, 0)) + fminf(fmaxf(cx, cy), 0) - 44;
    float a = fminf(fmaxf(0.5f - d, 0), 1);
    out[j][i][3] = (unsigned char)(255 * a);
  }
  if(argc > 2){ FILE *fp = fopen(argv[2], "wb"); fprintf(fp, "P6\n256 256\n255\n"); for(int j = 0; j < R; j++) for(int i = 0; i < R; i++){ float a = out[j][i][3] / 255.0f; for(int c = 0; c < 3; c++){ unsigned char v = (unsigned char)(out[j][i][c] * a + 90 * (1 - a)); fwrite(&v, 1, 1, fp); } } fclose(fp); }
  int sizes[] = {16, 32, 48, 64, 128, 256};
  for(int si = 0; si < 6; si++){
    int S = sizes[si], f = R / S;
    char nm[256]; snprintf(nm, sizeof nm, "%s/icon_%d.bin", dir, si + 1);
    FILE *fo = fopen(nm, "wb"); if(!fo){ perror(nm); return 1; }
    unsigned hdr[10] = {40, (unsigned)S, (unsigned)(S * 2), 0, 0, (unsigned)(S * S * 4 + S * ((S + 31) / 32) * 4), 0, 0, 0, 0};
    unsigned short pl = 1, bpp = 32;
    fwrite(&hdr[0], 4, 3, fo); fwrite(&pl, 2, 1, fo); fwrite(&bpp, 2, 1, fo); fwrite(&hdr[4], 4, 6, fo);
    for(int y = S - 1; y >= 0; y--) for(int x = 0; x < S; x++){
      float acc[4] = {0, 0, 0, 0};
      for(int dy = 0; dy < f; dy++) for(int dx = 0; dx < f; dx++){
        unsigned char *p = out[y * f + dy][x * f + dx]; float a = p[3] / 255.0f;
        acc[0] += p[0] * a; acc[1] += p[1] * a; acc[2] += p[2] * a; acc[3] += a;
      }
      float a = acc[3] / (f * f);
      unsigned char px[4];
      px[0] = (unsigned char)(acc[2] / (acc[3] > 0 ? acc[3] : 1)); /* B */
      px[1] = (unsigned char)(acc[1] / (acc[3] > 0 ? acc[3] : 1)); /* G */
      px[2] = (unsigned char)(acc[0] / (acc[3] > 0 ? acc[3] : 1)); /* R */
      px[3] = (unsigned char)(255 * a);
      fwrite(px, 1, 4, fo);
    }
    int rowb = ((S + 31) / 32) * 4; unsigned char *z = calloc(rowb * S, 1); fwrite(z, 1, rowb * S, fo); free(z);
    fclose(fo);
  }
  return 0;
}

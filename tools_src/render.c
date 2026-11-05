/* render.c - software check renderer of the GL points (same rules as intro.asm)
 * input: dump of the test bench (sim); output: one PNG per frame
 * usage: render dump.bin prefix [width height] [ref_pt]                  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static uint32_t crc_tab[256];
static void crc_init(void) { for (uint32_t n = 0; n < 256; n++) { uint32_t c = n; for (int k = 0; k < 8; k++) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1; crc_tab[n] = c; } }
static uint32_t crc(uint32_t c, const uint8_t *b, size_t n) { c ^= 0xFFFFFFFFu; while (n--) c = crc_tab[(c ^ *b++) & 255] ^ (c >> 8); return c ^ 0xFFFFFFFFu; }
static void put32(uint8_t *p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v; }
static void chunk(FILE *f, const char *t, const uint8_t *d, uint32_t n)
{
    uint8_t h[8]; put32(h, n); memcpy(h + 4, t, 4); fwrite(h, 1, 8, f);
    if (n) fwrite(d, 1, n, f);
    uint32_t c = crc(0, (const uint8_t *)t, 4); /* crc over type+data */
    uint8_t *tmp = malloc(n + 4); memcpy(tmp, t, 4); if (n) memcpy(tmp + 4, d, n);
    c = crc(0, tmp, n + 4); free(tmp);
    uint8_t e[4]; put32(e, c); fwrite(e, 1, 4, f);
}
static void write_png(const char *name, const uint8_t *rgb, int w, int h)
{
    FILE *f = fopen(name, "wb"); if (!f) return;
    fwrite("\x89PNG\r\n\x1a\n", 1, 8, f);
    uint8_t ih[13]; put32(ih, w); put32(ih + 4, h); ih[8] = 8; ih[9] = 2; ih[10] = ih[11] = ih[12] = 0;
    chunk(f, "IHDR", ih, 13);
    size_t rowb = (size_t)w * 3 + 1, raw = rowb * h;
    uint8_t *r = malloc(raw);
    for (int y = 0; y < h; y++) { r[y * rowb] = 0; memcpy(r + y * rowb + 1, rgb + (size_t)y * w * 3, (size_t)w * 3); }
    size_t nb = (raw + 65534) / 65535, zl = 2 + raw + nb * 5 + 4;
    uint8_t *z = malloc(zl); size_t o = 0; z[o++] = 0x78; z[o++] = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < raw; i++) { a = (a + r[i]) % 65521; b = (b + a) % 65521; }
    for (size_t p = 0; p < raw; p += 65535) {
        size_t n = raw - p < 65535 ? raw - p : 65535;
        z[o++] = (p + n >= raw) ? 1 : 0; z[o++] = n & 255; z[o++] = n >> 8; z[o++] = (~n) & 255; z[o++] = ((~n) >> 8) & 255;
        memcpy(z + o, r + p, n); o += n;
    }
    put32(z + o, (b << 16) | a); o += 4;
    chunk(f, "IDAT", z, (uint32_t)o); chunk(f, "IEND", NULL, 0);
    fclose(f); free(r); free(z);
}

static float *acc;
static int W, H;
static void splat(float sx, float sy, float rad, float r, float g, float b)
{
    if (rad < 0.5f) rad = 0.5f;
    int x0 = (int)floorf(sx - rad - 1), x1 = (int)ceilf(sx + rad + 1), y0 = (int)floorf(sy - rad - 1), y1 = (int)ceilf(sy + rad + 1);
    if (x1 < 0 || y1 < 0 || x0 >= W || y0 >= H) return;
    float area = 3.14159f * rad * rad; if (area < 1.0f) area = 1.0f; /* a point < 1 px covers 1 px */
    float norm = (rad < 0.75f) ? (3.14159f * rad * rad) : 1.0f; if (norm > 1) norm = 1;
    (void)area;
    for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) {
        if (x < 0 || y < 0 || x >= W || y >= H) continue;
        float dx = x + 0.5f - sx, dy = y + 0.5f - sy;
        float d = sqrtf(dx * dx + dy * dy);
        float cov = rad + 0.5f - d; /* antialiased disc */
        if (cov <= 0) continue; if (cov > 1) cov = 1;
        cov *= (rad < 0.75f) ? norm : 1.0f;
        float *p = acc + ((size_t)y * W + x) * 3;
        p[0] += r * cov; p[1] += g * cov; p[2] += b * cov;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) return 1;
    crc_init();
    W = argc > 3 ? atoi(argv[3]) : 960; H = argc > 4 ? atoi(argv[4]) : 540;
    float ptref = argc > 5 ? (float)atof(argv[5]) : 6.0f;
    FILE *f = fopen(argv[1], "rb"); if (!f) { perror("dump"); return 1; }
    acc = malloc(sizeof(float) * W * H * 3);
    uint8_t *rgb = malloc(W * H * 3);
    for (;;) {
        uint32_t hd[16];
        if (fread(hd, 4, 16, f) != 16) break;
        int n = hd[1];
        float rx = *(float *)&hd[2], ry = *(float *)&hd[3], d = *(float *)&hd[4], ps = *(float *)&hd[5], fade = *(float *)&hd[6];
        float *v = malloc(12 * (size_t)n + 16); uint32_t *c = malloc(4 * (size_t)n + 16);
        if (fread(v, 12, n, f) != (size_t)n || fread(c, 4, n, f) != (size_t)n) break;
        memset(acc, 0, sizeof(float) * W * H * 3);
        float aspect = (float)W / H, fl = 1.7320508f;
        float rx0 = rx, ry0 = ry, d0 = d, roll0 = *(float *)&hd[12];
        int split = hd[8];
        float bg_rx = *(float *)&hd[9], bg_ry = *(float *)&hd[10], bg_d = *(float *)&hd[11];
        float scl = H / 720.0f; int only = getenv("ONLY") ? atoi(getenv("ONLY")) : -1;
        for (int pass = 0; pass < 2; pass++) {
            float glow = pass == 0 ? 2.2f : 1.0f, amul = pass == 0 ? 0.18f : 1.0f;
            for (int i = 0; i < n; i++) {
                if (only == 0 && i >= split) continue; if (only == 1 && i < split) continue;
                float x = v[3 * i], y = v[3 * i + 1], z = v[3 * i + 2];
                if (i < split) { rx = bg_rx; ry = bg_ry; d = bg_d; } else { rx = rx0; ry = ry0; d = d0; }
                float crx = cosf(rx * 3.14159265f / 180), srx = sinf(rx * 3.14159265f / 180);
                float cry = cosf(ry * 3.14159265f / 180), sry = sinf(ry * 3.14159265f / 180);
                /* GL : T(0,0,-d) * Rx(rx) * Ry(ry) */
                float x1 = cry * x + sry * z, z1 = -sry * x + cry * z, y1 = y;
                float y2 = crx * y1 - srx * z1, z2 = srx * y1 + crx * z1, x2 = x1;
                if (i >= split) { float cr_ = cosf(roll0*3.14159265f/180), sr_ = sinf(roll0*3.14159265f/180); /* roll: applied after Rx Ry, before translation */ float xx_ = cr_*x2 - sr_*y2, yy_ = sr_*x2 + cr_*y2; x2 = xx_; y2 = yy_; }
                float zz = z2 - d;
                if (zz > -0.1f) continue;
                float dist = -zz;
                float sx = (x2 * fl / aspect) / dist, sy = (y2 * fl) / dist;
                float px = (sx * 0.5f + 0.5f) * W, py = (0.5f - sy * 0.5f) * H;
                float size = ps * scl * glow * (ptref / dist);
                float minsz = 1.0f * scl; if (size < minsz) size = minsz;
                uint32_t cc = c[i];
                float cr = (cc & 255) / 255.0f, cg = ((cc >> 8) & 255) / 255.0f, cb = ((cc >> 16) & 255) / 255.0f, ca = (cc >> 24) / 255.0f * amul;
                splat(px, py, size * 0.5f, cr * ca, cg * ca, cb * ca);
            }
        }
        for (int i = 0; i < W * H * 3; i++) {
            float vv = acc[i] * (1.0f - fade); if (vv > 1) vv = 1;
            rgb[i] = (uint8_t)(vv * 255.0f + 0.5f);
        }
        { const char *pf = getenv("PFX"); if (pf) { float zm, bl, lv, px_; if (sscanf(pf, "%f,%f,%f,%f", &zm, &bl, &lv, &px_) == 4) {
            uint8_t *src = malloc((size_t)W*H*3); memcpy(src, rgb, (size_t)W*H*3);
            #define TX(u,v,ch) ((float)src[((int)fminf(fmaxf((v)*H,0),H-1)*W + (int)fminf(fmaxf((u)*W,0),W-1))*3+ch]/255.0f)
            for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) {
                float u = (x+0.5f)/W, v = (y+0.5f)/H;
                if (px_ > 1) { float qx = px_, qy = px_*H/(float)W*(float)W/ (float)W * (float)W/ (float)W; qy = px_*(float)H/(float)W; u = (floorf(u*qx)+0.5f)/qx; v = (floorf(v*qy)+0.5f)/qy; }
                float c[3] = {0,0,0}, s = 0, dx = 0.5f-u, dy = 0.5f-v;
                for (int i = 0; i < 7; i++) { float f = i/6.0f, w = 1-0.45f*f; for (int k = 0; k < 3; k++) c[k] += w*TX(u+dx*f*zm, v+dy*f*zm, k); s += w; }
                for (int k = 0; k < 3; k++) c[k] /= s;
                float pxx = 1.0f/W, pyy = 1.0f/H; static const int o1[8][2] = {{4,0},{-4,0},{0,4},{0,-4},{3,3},{-3,-3},{3,-3},{-3,3}};
                for (int k = 0; k < 3; k++) { float g = 0, h = 0; for (int j = 0; j < 8; j++) { g += TX(u+o1[j][0]*pxx, v+o1[j][1]*pyy, k); h += TX(u+o1[j][0]*3*pxx, v+o1[j][1]*3*pyy, k); } c[k] += bl*(g*0.0625f + h*0.0625f); }
                for (int k = 0; k < 3; k++) { float cv = c[k]; if (lv > 1) cv = floorf(cv*lv+0.5f)/lv; if (cv > 1) cv = 1; rgb[(y*W+x)*3+k] = (uint8_t)(cv*255+0.5f); }
            }
            free(src); } } }
        if (getenv("RAW")) { char nr[512]; snprintf(nr, sizeof nr, "%s_%05u.rgb", argv[2], hd[0]); FILE *fr = fopen(nr, "wb"); if (fr) { fwrite(rgb, 1, (size_t)W*H*3, fr); fclose(fr); } }
        char name[512]; snprintf(name, sizeof name, "%s_%05u.png", argv[2], hd[0]);
        write_png(name, rgb, W, H);
        { double s=0; long b=0; for (int i=0;i<W*H;i++){ double l=0.3*rgb[i*3]+0.59*rgb[i*3+1]+0.11*rgb[i*3+2]; s+=l; if(l>60)b++; } printf("LUM %.1f clair %.1f%%\n", s/(W*H), 100.0*b/(W*H)); }
        printf("%s  (scene %u, %d points)\n", name, hd[7], n);
        free(v); free(c);
    }
    return 0;
}

/* adpcm.c: stereo IMA ADPCM encoder/decoder (1 byte per stereo sample: L | R<<4)
   usage : adpcm enc in.s16 out.adp [tab.inc]   |   adpcm dec in.adp out.s16     coder : Ivandoe */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static const int stp[89]={7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767};
static const int itab[8]={-1,-1,-1,-1,2,4,6,8};
typedef struct{int p,i;}St;
static int dec1(St*s,int n){int step=stp[s->i];int d=step>>3;if(n&4)d+=step;if(n&2)d+=step>>1;if(n&1)d+=step>>2;
 if(n&8)s->p-=d;else s->p+=d;if(s->p>32767)s->p=32767;if(s->p<-32768)s->p=-32768;s->i+=itab[n&7];if(s->i<0)s->i=0;if(s->i>88)s->i=88;return s->p;}
static int enc1(St*s,int x){int step=stp[s->i];int diff=x-s->p;int n=0;if(diff<0){n=8;diff=-diff;}
 if(diff>=step){n|=4;diff-=step;}if(diff>=(step>>1)){n|=2;diff-=step>>1;}if(diff>=(step>>2))n|=1;
 dec1(s,n);return n;}
int main(int c,char**v){
 if(!strcmp(v[1],"enc")){FILE*f=fopen(v[2],"rb");fseek(f,0,SEEK_END);long b=ftell(f);fseek(f,0,SEEK_SET);int16_t*p=malloc(b);fread(p,1,b,f);fclose(f);
  long n=b/4;uint8_t*o=malloc(n);St l={0,0},r={0,0};double e=0,sg=0;
  for(long i=0;i<n;i++){int a=enc1(&l,p[2*i]),d=enc1(&r,p[2*i+1]);o[i]=a|(d<<4);double x=l.p-p[2*i],y=r.p-p[2*i+1];e+=x*x+y*y;sg+=(double)p[2*i]*p[2*i]+(double)p[2*i+1]*p[2*i+1];}
  f=fopen(v[3],"wb");fwrite(o,1,n,f);fclose(f);fprintf(stderr,"%ld samples, SNR %.1f dB\n",n,10*log10(sg/e));
  if(c>4){f=fopen(v[4],"w");fprintf(f,"adp_step:\n");for(int i=0;i<89;i++)fprintf(f," dw %d\n",stp[i]);fprintf(f,"adp_idx: db -1,-1,-1,-1,2,4,6,8\n");fclose(f);}
 } else {FILE*f=fopen(v[2],"rb");fseek(f,0,SEEK_END);long n=ftell(f);fseek(f,0,SEEK_SET);uint8_t*a=malloc(n);fread(a,1,n,f);fclose(f);
  St l={0,0},r={0,0};f=fopen(v[3],"wb");for(long i=0;i<n;i++){int16_t s[2];s[0]=dec1(&l,a[i]&15);s[1]=dec1(&r,a[i]>>4);fwrite(s,2,2,f);}fclose(f);}
 return 0;}

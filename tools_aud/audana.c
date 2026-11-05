/* audana.c - analysis of a PCM song (s16 stereo 44100) to sync the demo at 50 Hz:
   4 bands (bass / mids / treble / total): level, attack age, pitch, instrument;
   tempo, beat phase, first beat of bar, row grid (1/4 beat) and
   novelty (section changes) to align the scenes on the bars.
   usage: audana pcm.s16 output_dir     coder: Ivandoe */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#define SPF 882
#define NF 2048
#define MAXF 30000
static double wnd[NF];
static void fft(double*re,double*im,int n){
 for(int i=1,j=0;i<n;i++){int b=n>>1;for(;j&b;b>>=1)j^=b;j^=b;if(i<j){double t=re[i];re[i]=re[j];re[j]=t;t=im[i];im[i]=im[j];im[j]=t;}}
 for(int l=2;l<=n;l<<=1){double a=-2*M_PI/l;double wr=cos(a),wi=sin(a);for(int i=0;i<n;i+=l){double cr=1,ci=0;for(int j=0;j<l/2;j++){
   int u=i+j,v=i+j+l/2;double xr=re[v]*cr-im[v]*ci,xi=re[v]*ci+im[v]*cr;re[v]=re[u]-xr;im[v]=im[u]-xi;re[u]+=xr;im[u]+=xi;double t=cr*wr-ci*wi;ci=cr*wi+ci*wr;cr=t;}}}}
static int cmpd(const void*a,const void*b){double x=*(double*)a,y=*(double*)b;return x<y?-1:x>y;}
static double pct(double*a,int n,double p){double*t=malloc(n*8);memcpy(t,a,n*8);qsort(t,n,8,cmpd);double r=t[(int)(p*(n-1))];free(t);return r;}
int main(int argc,char**argv){
 FILE*f=fopen(argv[1],"rb"); fseek(f,0,SEEK_END); long bytes=ftell(f); fseek(f,0,SEEK_SET);
 long ns=bytes/4; int16_t*pcm=malloc(bytes); fread(pcm,1,bytes,f); fclose(f);
 char nm[512]; int nfr=(int)(ns/SPF); if(nfr>MAXF)nfr=MAXF;
 for(int i=0;i<NF;i++)wnd[i]=0.5-0.5*cos(2*M_PI*i/(NF-1));
 const int bl[4]={2,10,40,2},bh[4]={10,100,400,1000}; /* bins (21.5 Hz/bin): 43-215 Hz, 215-2150 Hz, 860-8600 Hz... */
 /* bands in bins: b0 30-150 Hz, b1 150-1800, b2 3000-12000, b3 all */
 int lo[4]={2,8,140,1},hi[4]={7,84,560,1023};
 static double E[4][MAXF],FL[4][MAXF],CE[4][MAXF]; static double prev[1024];
 double re[NF],im[NF];
 for(int fr=0;fr<nfr;fr++){
  long c=(long)fr*SPF+SPF/2; 
  for(int i=0;i<NF;i++){long k=c-NF/2+i;double s=0;if(k>=0&&k<ns)s=(pcm[2*k]+pcm[2*k+1])*0.5/32768.0;re[i]=s*wnd[i];im[i]=0;}
  fft(re,im,NF);
  double mag[1024];for(int k=0;k<1024;k++)mag[k]=sqrt(re[k]*re[k]+im[k]*im[k]);
  for(int b=0;b<4;b++){double e=0,fl=0,ce=0,cw=0;for(int k=lo[b];k<=hi[b];k++){e+=mag[k]*mag[k];double d=mag[k]-prev[k];if(d>0)fl+=d;ce+=mag[k]*mag[k]*log2(k*21.53+1);cw+=mag[k]*mag[k];}
   E[b][fr]=10*log10(e+1e-9);FL[b][fr]=fl;CE[b][fr]=cw>0?ce/cw:0;}
  memcpy(prev,mag,sizeof prev);}
 /* normalization + attack detection */
 uint8_t*lvl=malloc(nfr*4),*age=malloc(nfr*4),*pit=malloc(nfr*4),*ins=malloc(nfr*4); static int onset[4][MAXF]; 
 const int insc[4]={8,11,9,0};
 for(int b=0;b<4;b++){
  double p5=pct(E[b],nfr,0.05),p98=pct(E[b],nfr,0.98); double pf=pct(FL[b],nfr,0.95);
  double clo=1e9,chi=0;for(int i=0;i<nfr;i++){if(CE[b][i]>0){if(CE[b][i]<clo)clo=CE[b][i];if(CE[b][i]>chi)chi=CE[b][i];}}
  double c5=pct(CE[b],nfr,0.05),c95=pct(CE[b],nfr,0.95);
  int last=-100;
  for(int i=0;i<nfr;i++){
   double v=(E[b][i]-p5)/(p98-p5);if(v<0)v=0;if(v>1)v=1;lvl[i*4+b]=(uint8_t)(v*255);
   double p=(CE[b][i]-c5)/(c95-c5+1e-9);if(p<0)p=0;if(p>1)p=1;pit[i*4+b]=(uint8_t)(p*255);
   /* adaptive threshold: local mean over +-25 frames */
   double m=0;int n=0;for(int j=i-25;j<=i+25;j++)if(j>=0&&j<nfr){m+=FL[b][j];n++;}m/=n;
   int pk=FL[b][i]>m*1.6+0.04*pf; if(i>0&&FL[b][i]<FL[b][i-1])pk=0; if(i+1<nfr&&FL[b][i]<FL[b][i+1])pk=0;
   if(b==3)pk=pk&&FL[b][i]>m*1.9+0.08*pf;
   if(pk&&i-last>=4){onset[b][i]=1;last=i;}
   int a=i-last;if(a>255)a=255;age[i*4+b]=a;ins[i*4+b]=insc[b];}}
 /* attack strength for the tempo: bass + mids */
 static double O[MAXF];double mo=0;for(int i=0;i<nfr;i++){O[i]=FL[0][i]*1.0/ (pct(FL[0],nfr,0.95)+1e-9)+0.7*FL[1][i]/(pct(FL[1],nfr,0.95)+1e-9)+0.3*FL[2][i]/(pct(FL[2],nfr,0.95)+1e-9);}
 /* tempo + phase: comb over the attack strength */
 double bestS=-1,bestB=120,bestP=0;
 for(double bpm=70;bpm<=190;bpm+=0.01){double P=3000.0/bpm;int nb=(int)((nfr-P)/P);
  for(double ph=0;ph<P;ph+=0.5){double s=0;for(int k=0;k<nb;k++){double t=ph+k*P;int i=(int)t;double fr=t-i;if(i+1>=nfr)break;s+=O[i]*(1-fr)+O[i+1]*fr;}
   s/=nb; if(s>bestS){bestS=s;bestB=bpm;bestP=ph;}}}
 double P=3000.0/bestB; int nb=(int)((nfr-bestP)/P);
 fprintf(stderr,"tempo %.2f BPM (beat = %.3f frames), phase %.1f, strength %.3f\n",bestB,P,bestP,bestS);
 /* drift check: best local phase per 20 s slice */
 for(int seg=0;seg*1000<nfr;seg++){double bs=-1,bp=0;for(double d=-3;d<=3;d+=0.25){double s=0;int n=0;for(int k=0;k<nb;k++){double t=bestP+k*P;if(t<seg*1000||t>=(seg+1)*1000)continue;double tt=t+d;int i=(int)tt;if(i<0||i+1>=nfr)continue;s+=O[i];n++;}if(n&&s/n>bs){bs=s/n;bp=d;}}
  fprintf(stderr,"  slice %2d: local offset %+.2f frames\n",seg,bp);}
 /* first beat of the bar: try the 4 positions, bass + novelty */
 /* novelty: difference of mean levels between 4 s before and 4 s after */
 static double NV[MAXF];
 for(int i=0;i<nfr;i++){double d=0;for(int b=0;b<3;b++){double a=0,c=0;int na=0,nc=0;for(int j=i-200;j<i;j++)if(j>=0){a+=lvl[j*4+b];na++;}for(int j=i;j<i+200;j++)if(j<nfr){c+=lvl[j*4+b];nc++;}if(na&&nc)d+=fabs(a/na-c/nc);}NV[i]=d;}
 double sc[4]={0,0,0,0},sn[4]={0,0,0,0};
 for(int k=0;k<nb;k++){double t=bestP+k*P;int i=(int)(t+0.5);if(i>=nfr)break;int o=k%4;
  double e=0;for(int j=i-1;j<=i+1;j++)if(j>=0&&j<nfr)e+=FL[0][j];sc[o]+=e;sn[o]+=NV[i];}
 int bo=0;double bq=-1;double smax=0,nmax=0;for(int o=0;o<4;o++){if(sc[o]>smax)smax=sc[o];if(sn[o]>nmax)nmax=sn[o];}
 for(int o=0;o<4;o++){double q=sc[o]/smax+1.5*sn[o]/nmax;fprintf(stderr,"  temps %d : grave %.2f nouveaute %.2f -> %.2f\n",o,sc[o]/smax,sn[o]/nmax,q);if(q>bq){bq=q;bo=o;}}
 /* ---- dynamic beat tracking: dynamic programming on the local offset (+-5 frames) ---- */
 int ND=21; static double tb[4000]; static double cost[4000][21]; static int from[4000][21];
 { double lam=0.04*bestS; 
   for(int k=0;k<nb;k++)for(int d=0;d<ND;d++){double t=bestP+k*P+(d-10)*0.5;int i=(int)t;double fr=t-i;double o=0;if(i>=0&&i+1<nfr)o=O[i]*(1-fr)+O[i+1]*fr;
    double best=0;int bf=0;if(k>0){best=1e18;for(int e=0;e<ND;e++){double c=cost[k-1][e]+lam*((d-e)*0.5)*((d-e)*0.5);if(c<best){best=c;bf=e;}}}
    cost[k][d]=best-o;from[k][d]=bf;}
   int bd=0;double bc=1e18;for(int d=0;d<ND;d++)if(cost[nb-1][d]<bc){bc=cost[nb-1][d];bd=d;}
   for(int k=nb-1;k>=0;k--){tb[k]=bestP+k*P+(bd-10)*0.5;bd=from[k][bd];}
   double mx=0;for(int k=0;k<nb;k++){double dd=fabs(tb[k]-(bestP+k*P));if(dd>mx)mx=dd;}fprintf(stderr,"beat tracking: max deviation from the grid %.1f images\n",mx);}
 int k0=bo; 
 /* row positions: 4 rows per beat, row 64 = first beat of bar k0 */
 uint16_t*pos=malloc(nfr*2);
 for(int i=0;i<nfr;i++){double v;int k=-1;
  if(i<tb[0]){double t0=tb[0]-P;v=64+4*(-1-k0)+floor(4*(i-t0)/P);}
  else{for(k=0;k+1<nb&&tb[k+1]<=i;k++);double Pk=(k+1<nb)?tb[k+1]-tb[k]:P;v=64+4*(k-k0)+floor(4*(i-tb[k])/Pk);if(v>64+4*(k-k0)+3)v=64+4*(k-k0)+3;}
  if(v<0)v=0;pos[i]=(uint16_t)v;}
 double bar=P*4,fd=tb[k0],R=P/4;
 const char*nn[5]={"t_lvl.bin","t_age.bin","t_pit.bin","t_ins.bin","t_pos.bin"};
 for(int k=0;k<5;k++){snprintf(nm,sizeof nm,"%s/%s",argv[2],nn[k]);f=fopen(nm,"wb");fwrite(k==0?lvl:k==1?age:k==2?pit:k==3?ins:(uint8_t*)pos,1,k==4?nfr*2:nfr*4,f);fclose(f);}
 snprintf(nm,sizeof nm,"%s/track.inc",argv[2]);f=fopen(nm,"w");fprintf(f,"%%define TRK_NFR %d\n",nfr);fclose(f);
 /* bars: start (frame), novelty */
 snprintf(nm,sizeof nm,"%s/bars.txt",argv[2]);f=fopen(nm,"w");
 for(int m=0;;m++){int k=k0+4*m;if(k>=nb)break;int i=(int)ceil(tb[k]);fprintf(f,"%d %d %.3f %d\n",m,i,NV[i<nfr?i:nfr-1],64+16*m);}
 fclose(f);
 int on[4]={0,0,0,0};for(int b=0;b<4;b++)for(int i=0;i<nfr;i++)on[b]+=onset[b][i];
 fprintf(stderr,"frames %d (%.1f s); attacks: bass %d, mids %d, treble %d, total %d\n",nfr,nfr/50.0,on[0],on[1],on[2],on[3]);
 return 0;}

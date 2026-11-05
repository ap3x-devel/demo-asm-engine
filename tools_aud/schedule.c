/* schedule.c: aligns the scenes on the bars. usage: schedule bars.txt original_starts.txt nfr > scenes_new.txt
   output: one line per scene: order row frame   coder: Ivandoe */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(int c,char**v){
 static int bf[400],br[400];static double nv[400];int nb=0;FILE*f=fopen(v[1],"r");int m,fr,rw;double n;
 while(fscanf(f,"%d %d %lf %d",&m,&fr,&n,&rw)==4){bf[nb]=fr;nv[nb]=n;br[nb]=rw;nb++;}fclose(f);
 double mx=0;for(int i=0;i<nb;i++)if(nv[i]>mx)mx=nv[i];
 static double s[64];int N=0;f=fopen(v[2],"r");while(fscanf(f,"%lf",&s[N])==1)N++;fclose(f);
 int NF=atoi(v[3]);double E0=87152,S1=1011;s[N]=E0;
 double tot=0,w[64];for(int i=3;i<N;i++){w[i]=sqrt(s[i+1]-s[i]);tot+=w[i];}
 int sel[64];int tit[4]={0,2,4,6};for(int i=0;i<4;i++)sel[i]=tit[i];
 double t=bf[6];double span=NF-t-120; /* the last scene lasts at least ~2.4 s */
 for(int i=4;i<N;i++){}
 /* scenes 3..N-1: scene 3 is at bar 6; ideal time of the following ones */
 double ideal[64];t=bf[6];for(int i=3;i<N;i++){ideal[i]=t;t+=w[i]*(NF-bf[6])/tot;}
 int prev=6;
 for(int i=4;i<N;i++){int best=-1;double bq=1e9;for(int b=prev+1;b<nb;b++){double d=fabs(bf[b]-ideal[i])/152.0;if(d>1.6)continue;
   double q=d-0.9*nv[b]/mx; if(bf[b]>NF-150)continue; if(q<bq){bq=q;best=b;}}
  if(best<0){best=prev+1;} sel[i]=best;prev=best;}
 for(int i=0;i<N;i++)printf("%d %d %d\n",br[sel[i]]/64,br[sel[i]]%64,bf[sel[i]]);
 return 0;}

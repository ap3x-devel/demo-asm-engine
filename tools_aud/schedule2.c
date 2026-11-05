/* schedule2.c: picks the starts of N content scenes on the bars (DP: novelty + regularity).
   usage: schedule2 bars.txt NF N > starts.txt  (3 titles at bars 0,2,4 then N scenes from bar 6)
   output: one line per scene: order row frame   coder: Ivandoe */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(int c,char**v){
 static int bf[400],br[400];static double nv[400];int nb=0;FILE*f=fopen(v[1],"r");int m,fr,rw;double n;
 while(fscanf(f,"%d %d %lf %d",&m,&fr,&n,&rw)==4){bf[nb]=fr;nv[nb]=n;br[nb]=rw;nb++;}fclose(f);
 int NF=atoi(v[2]),N=atoi(v[3]);
 double mx=0;for(int i=0;i<nb;i++)if(nv[i]>mx)mx=nv[i];
 int B0=6, BE=nb; /* virtual end = nb */
 while(BE>B0 && bf[BE-1]>NF-150) BE--;   /* last possible start bar */
 double ideal=(double)(nb-B0)/N;
 static double dp[40][400];static int pv[40][400];
 for(int k=0;k<40;k++)for(int b=0;b<400;b++){dp[k][b]=-1e9;pv[k][b]=-1;}
 dp[0][B0]=0;
 for(int k=1;k<N;k++)for(int b=B0+1;b<BE;b++)for(int a=b-6;a<=b-2;a++){ if(a<B0||dp[k-1][a]<-1e8)continue;
   double g=b-a, q=dp[k-1][a]+nv[b]/mx - 0.45*(g-ideal)*(g-ideal);
   if(q>dp[k][b]){dp[k][b]=q;pv[k][b]=a;} }
 double best=-1e9;int bb=-1;
 for(int b=B0+1;b<BE;b++){ double g=nb-b; if(g<2||g>6)continue; double q=dp[N-1][b]-0.45*(g-ideal)*(g-ideal); if(q>best){best=q;bb=b;} }
 if(bb<0){fprintf(stderr,"pas de solution\n");return 1;}
 int sel[40];int b=bb;for(int k=N-1;k>=0;k--){sel[k]=b;b=pv[k][b];}
 int tit[3]={0,2,4};
 for(int i=0;i<3;i++)printf("%d %d %d\n",br[tit[i]]/64,br[tit[i]]%64,bf[tit[i]]);
 for(int k=0;k<N;k++)printf("%d %d %d\n",br[sel[k]]/64,br[sel[k]]%64,bf[sel[k]]);
 return 0;}

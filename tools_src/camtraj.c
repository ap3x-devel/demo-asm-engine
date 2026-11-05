/* reads the sim dump on stdin and prints: frame scene rx ry d roll (camera trajectory check) */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
int main(void){
  uint32_t hd[16]; static char buf[1<<24];
  while(fread(hd,4,16,stdin)==16){
    size_t n=hd[1]; if(n*16>sizeof buf) return 1;
    if(fread(buf,16,n,stdin)!=n) break;
    float rx,ry,d,ro,co,ct; memcpy(&co,&hd[13],4);memcpy(&ct,&hd[14],4);memcpy(&rx,&hd[2],4);memcpy(&ry,&hd[3],4);memcpy(&d,&hd[4],4);memcpy(&ro,&hd[12],4);
    printf("%u %u %.3f %.3f %.3f %.3f %.3f %.3f\n",hd[0],hd[7],rx,ry,d,ro,co,ct);
  }
  return 0;
}

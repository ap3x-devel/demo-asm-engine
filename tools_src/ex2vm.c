/* ex2vm: translator of infix expressions -> stack machine macros (NASM).
   Input: lines "var = expression" or "expression" (stays on the stack); "#" comment.
   Variables: t0..t15; xx/zz -> macro parameters %1/%2; others -> V_NAME.
   Coder: Ivandoe */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
static const char *p;
static void expr(void);
static void ws(void){ while(*p==' '||*p=='\t') p++; }
static void ident_load(const char *id){
  if(!strcmp(id,"xx")) printf(" ldv %%1\n");
  else if(!strcmp(id,"zz")) printf(" ldv %%2\n");
  else if(id[0]=='t' && isdigit((unsigned char)id[1])) printf(" ldv %s\n",id);
  else { char b[64]; int i; for(i=0;id[i];i++) b[i]=toupper((unsigned char)id[i]); b[i]=0; printf(" ldv V_%s\n",b); }
}
static void call(const char *f){
  int n=0; char op[32]; int oct=-1;
  ws(); if(*p!='(') { fprintf(stderr,"( expected after %s\n",f); exit(1); } p++;
  ws();
  if(*p!=')') for(;;){
    ws();
    /* last integer argument of fbm/rdg: octaves */
    if((!strcmp(f,"fbm")||!strcmp(f,"rdg")) && n==3){ oct=(int)strtol(p,(char**)&p,10); n++; }
    else { expr(); n++; }
    ws(); if(*p==',') { p++; continue; } break;
  }
  ws(); if(*p!=')') { fprintf(stderr,") expected\n"); exit(1);} p++;
  if(!strcmp(f,"fbm")||!strcmp(f,"rdg")) { printf(" o_%s %d\n",f,oct); return; }
  if(!strcmp(f,"mix")) strcpy(op,"mix"); else strcpy(op,f);
  printf(" o_%s\n",op);
}
static void prim(void){
  ws();
  if(*p=='('){ p++; expr(); ws(); if(*p!=')'){fprintf(stderr,") missing\n");exit(1);} p++; return; }
  if(*p=='-'){ p++; prim(); printf(" o_neg\n"); return; }
  if(isdigit((unsigned char)*p)||*p=='.'){ char *e; double v=strtod(p,&e); p=e; { char b[40]; snprintf(b,sizeof b,"%.9g",v); if(!strpbrk(b,".eEn")) strcat(b,".0"); printf(" kk %s\n",b); } return; }
  if(isalpha((unsigned char)*p)){ char id[32]; int n=0; while(isalnum((unsigned char)*p)||*p=='_') id[n++]=*p++; id[n]=0;
    ws(); if(*p=='(') call(id); else ident_load(id); return; }
  fprintf(stderr,"syntax error: %s\n",p); exit(1);
}
static void pw(void){ prim(); ws(); if(*p=='^'){ p++; pw(); printf(" o_pow\n"); } }
static void term(void){ pw(); for(;;){ ws(); if(*p=='*'){p++;pw();printf(" o_mul\n");} else if(*p=='/'){p++;pw();printf(" o_div\n");} else break; } }
static void expr(void){ term(); for(;;){ ws(); if(*p=='+'){p++;term();printf(" o_add\n");} else if(*p=='-'){p++;term();printf(" o_sub\n");} else break; } }
int main(void){
  char line[1024];
  while(fgets(line,sizeof line,stdin)){
    char *h=strchr(line,'#'); if(h)*h=0;
    char *s=line; while(isspace((unsigned char)*s)) s++;
    if(!*s) continue;
    /* assignment? */
    char id[32]; int n=0; char *q=s;
    while(isalnum((unsigned char)*q)||*q=='_') { if(n<31) id[n++]=*q; q++; }
    id[n]=0; char *r=q; while(*r==' ') r++;
    if(n && *r=='=' && r[1]!='='){ p=r+1; expr();
      if(!strcmp(id,"xx")||!strcmp(id,"zz")){fprintf(stderr,"assignment not allowed\n");return 1;}
      if(id[0]=='t'&&isdigit((unsigned char)id[1])) printf(" stv %s\n",id);
      else { char b[64]; int i; for(i=0;id[i];i++) b[i]=toupper((unsigned char)id[i]); b[i]=0; printf(" stv V_%s\n",b); } }
    else { p=s; expr(); }
  }
  return 0;
}

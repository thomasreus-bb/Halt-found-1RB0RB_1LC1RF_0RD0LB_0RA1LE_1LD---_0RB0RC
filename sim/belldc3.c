// belldc3.c - exact Bell orbit of the BB(6) holdout 1RB0RB_1LC1RF_0RD0LB_0RA1LE_1LD---_0RB0RC
// (divide and conquer with GMP, base 3).
//
// Bell configuration (head at the left end in state D):  0 11 0 1 0 1^n W (0 1 0 11)^m
//   n: long block, W: short word (192 words), m: counter of units 01011.
// Bell map: n' = 4*floor(n/3) + c2, (W', dm, c2) = f(W, min(m,3), n mod 3)   (table tab3.txt, derived from the TM).
// Bell 1 is n = 4, W = empty, m = 1 (blank tape after a few steps).
//
// Block of B Bells: n = q*3^B + r  ->  after B Bells  n = q*4^B + r_B, r_B from r alone (recursively).
// 4^B is a shift, so the costly part is the division by 3^B.
//
// Compile: gcc -O3 -march=native -o belldc3 belldc3.c -lgmp
// Usage:   ./belldc3 [-t TARGET] [-i INTERVAL]      (continues from the newest checkpoint in checkpoints3/)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>
#include <dirent.h>
#include <gmp.h>
typedef uint64_t u64;
#define LEAF 20
static int NW; static char **wname;
static int tw[400][4][3], tdm[400][4][3], tc[400][4][3];   // tw = -1: HALT, -2: missing
static int dist[400];
static int W=0; static long long M=1;
static long long gbell=0; static int halted=0; static long long haltbell=-1; static int haltW=-1; static long long haltM=0;
static int dmin=INT_MAX; static long long dminbell=-1;
static u64 p3[LEAF+1];
static double now(){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC,&ts); return ts.tv_sec+1e-9*ts.tv_nsec; }
static mpz_t R[80]; static struct { long long h; mpz_t v; } P3[80][2]; static int p3n[80];
static const mpz_t* pow3(long long h,int d){
  for(int i=0;i<2;i++) if(P3[d][i].h==h) return (const mpz_t*)&P3[d][i].v;
  int i=p3n[d]; p3n[d]^=1; P3[d][i].h=h; mpz_ui_pow_ui(P3[d][i].v,3,(unsigned long)h); return (const mpz_t*)&P3[d][i].v; }

// m Bells on x (exact), with x < 3^m * something: x = q*3^m + r
static void leaf(mpz_t x,int m){
  u64 r=mpz_fdiv_q_ui(x,x,(unsigned long)p3[m]);   // x <- q, r = x mod 3^m
  int done=0;
  for(int j=0;j<m;j++){
    int mk=M<3?(int)M:3; int rr=(int)(r%3);
    if(dist[W]<dmin){ dmin=dist[W]; dminbell=gbell; }
    int w2=tw[W][mk][rr];
    if(w2==-1){ halted=1; haltbell=gbell; haltW=W; haltM=M; return; }
    if(w2==-2){ fprintf(stderr,"missing table entry W=%s mk=%d r=%d at Bell %lld\n",wname[W],mk,rr,gbell); exit(3); }
    r=4*(r/3)+(u64)tc[W][mk][rr]; M+=tdm[W][mk][rr]; W=w2; gbell++; done++; }
  mpz_mul_2exp(x,x,2*(mp_bitcnt_t)done); mpz_add_ui(x,x,(unsigned long)r); }
static void adv(mpz_t x,long long m,int d){
  if(m<=LEAF){ leaf(x,(int)m); return; }
  long long h=m/2;
  mpz_fdiv_qr(x,R[d],x,*pow3(h,d));       // x = q, R = r
  adv(R[d],h,d+1); if(halted) return;
  mpz_mul_2exp(x,x,2*(mp_bitcnt_t)h); mpz_add(x,x,R[d]);
  adv(x,m-h,d+1); }

static u64 fnv(u64 h,const char*p,size_t k){ for(size_t i=0;i<k;i++){ h^=(unsigned char)p[i]; h*=1099511628211ULL; } return h; }
static int write_ckpt(long long bell,const mpz_t n){
  char name[256],tmp[300]; snprintf(name,sizeof name,"checkpoints3/bell_%lld_chkpt.txt",bell); snprintf(tmp,sizeof tmp,"%s.tmp",name);
  FILE*f=fopen(tmp,"w"); if(!f){ perror(tmp); return -1; }
  char*hx=mpz_get_str(NULL,16,n); char hdr[512];
  snprintf(hdr,sizeof hdr,"bell %lld\nW %s\nm %lld\n",bell,wname[W],M);
  u64 h=fnv(14695981039346656037ULL,hdr,strlen(hdr)); h=fnv(h,hx,strlen(hx));
  fprintf(f,"# belldc3 checkpoint (1RB0RB_1LC1RF_0RD0LB_0RA1LE_1LD---_0RB0RC)\n%sn_bits %zu\nn_hex %s\n",hdr,mpz_sizeinbase(n,2),hx);
  if(dmin<INT_MAX) fprintf(f,"dmin %d at_bell %lld\n",dmin,dminbell);
  fprintf(f,"checksum %016llx\n",(unsigned long long)h); free(hx);
  if(ferror(f)){ fclose(f); remove(tmp); return -1; } if(fclose(f)) return -1; return rename(tmp,name); }
static int read_ckpt(const char*name,long long*bell,mpz_t n){
  FILE*f=fopen(name,"r"); if(!f) return -1;
  char*line=NULL; size_t cap=0; ssize_t r; char wb[300]=""; long long b=-1,mm=-1; char*hx=NULL; unsigned long long cs=0; int have=0;
  while((r=getline(&line,&cap,f))>0){
    if(!strncmp(line,"bell ",5)) b=atoll(line+5);
    else if(!strncmp(line,"W ",2)) sscanf(line+2,"%299s",wb);
    else if(!strncmp(line,"m ",2)) mm=atoll(line+2);
    else if(!strncmp(line,"n_hex ",6)){ hx=strdup(line+6); hx[strcspn(hx,"\r\n")]=0; }
    else if(!strncmp(line,"dmin ",5)) sscanf(line+5,"%d at_bell %lld",&dmin,&dminbell);
    else if(!strncmp(line,"checksum ",9)){ cs=strtoull(line+9,NULL,16); have=1; } }
  free(line); fclose(f);
  if(b<0||mm<0||!hx||!have) return -1;
  int wi=-1; for(int i=0;i<NW;i++) if(!strcmp(wname[i],wb)) wi=i;
  if(wi<0) return -1;
  char hdr[512]; snprintf(hdr,sizeof hdr,"bell %lld\nW %s\nm %lld\n",b,wb,mm);
  u64 h=fnv(14695981039346656037ULL,hdr,strlen(hdr)); h=fnv(h,hx,strlen(hx));
  if(h!=cs){ fprintf(stderr,"%s: checksum does not match\n",name); free(hx); return -1; }
  mpz_set_str(n,hx,16); free(hx); *bell=b; W=wi; M=mm; return 0; }
static void logline(long long bell,const mpz_t n,double t,const char*what){
  char line[512]; time_t nt=time(NULL); char ts[32]; strftime(ts,sizeof ts,"%Y-%m-%d %H:%M:%S",localtime(&nt));
  snprintf(line,sizeof line,"%s  %s  bell %lld  time %.0fs  KCHECK %llu %zu W=%s m=%lld  d %d D %d (Bell %lld)\n",ts,what,bell,t,
           (unsigned long long)mpz_getlimbn(n,0),mpz_sizeinbase(n,2),wname[W][0]?wname[W]:"e",M,dist[W],dmin==INT_MAX?-1:dmin,dminbell);
  fputs(line,stdout); fflush(stdout); FILE*f=fopen("progress3.log","a"); if(f){ fputs(line,f); fclose(f); } }

int main(int argc,char**argv){
  long long target=LLONG_MAX, interval=100000000LL;
  for(int i=1;i<argc;i++){ if(!strcmp(argv[i],"-t")&&i+1<argc) target=atoll(argv[++i]); else if(!strcmp(argv[i],"-i")&&i+1<argc) interval=atoll(argv[++i]);
    else { fprintf(stderr,"usage: %s [-t TARGET] [-i INTERVAL]\n",argv[0]); return 2; } }
  FILE*f=fopen("tab3.txt","r"); int ne; if(!f||fscanf(f,"%d %d",&NW,&ne)!=2){ fprintf(stderr,"cannot read tab3.txt\n"); return 1; }
  wname=malloc(sizeof(char*)*NW); for(int i=0;i<NW;i++){ wname[i]=malloc(300); if(fscanf(f,"%299s",wname[i])!=1) return 1; if(!strcmp(wname[i],"-")) wname[i][0]=0; }
  for(int i=0;i<NW;i++) for(int a=0;a<4;a++) for(int b=0;b<3;b++) tw[i][a][b]=-2;
  for(int e=0;e<ne;e++){ int w,mk,r,dm,c2; char w2[16]; if(fscanf(f,"%d %d %d %15s %d %d",&w,&mk,&r,w2,&dm,&c2)!=6) return 1;
    if(w2[0]=='H') tw[w][mk][r]=-1; else { tw[w][mk][r]=atoi(w2); tdm[w][mk][r]=dm; tc[w][mk][r]=c2; } }
  fclose(f);
  for(int i=0;i<NW;i++) dist[i]=INT_MAX/2;
  for(int ch=1;ch;){ ch=0; for(int i=0;i<NW;i++) for(int r=0;r<3;r++){ int t=tw[i][3][r]; int v=t==-1?1:(t>=0&&dist[t]<INT_MAX/2?dist[t]+1:INT_MAX/2); if(v<dist[i]){dist[i]=v;ch=1;} } }
  p3[0]=1; for(int i=1;i<=LEAF;i++) p3[i]=p3[i-1]*3;
  for(int d=0;d<80;d++){ mpz_init(R[d]); for(int i=0;i<2;i++){ P3[d][i].h=-1; mpz_init(P3[d][i].v); } }
  mpz_t n,q,r,pw; mpz_inits(n,q,r,pw,NULL); long long pwB=-1;
  long long bell=-1; char start[300]="start (Bell 1)";
  { DIR*dir=opendir("checkpoints3"); if(dir){ struct dirent*e; long long bestb=-1; char bestn[300]="";
      while((e=readdir(dir))){ long long b; char rest[16]; if(sscanf(e->d_name,"bell_%lld_chkpt.tx%15s",&b,rest)==2&&!strcmp(rest,"t")&&b>bestb){ bestb=b; snprintf(bestn,sizeof bestn,"checkpoints3/%s",e->d_name);} }
      closedir(dir); if(bestb>=0&&read_ckpt(bestn,&bell,n)==0) snprintf(start,sizeof start,"%s",bestn); } }
  if(bell<0){ bell=1; mpz_set_ui(n,4); W=0; for(int i=0;i<NW;i++) if(!wname[i][0]) W=i; M=1; }
  { int mx=0; for(int i=0;i<NW;i++) if(dist[i]<INT_MAX/2&&dist[i]>mx) mx=dist[i];
    printf("belldc3: start %s, Bell %lld, target %lld; %d words, d(W) between 1 and %d\n",start,bell,target,NW,mx); }
  double t0=now(); long long nextck=(bell/interval+1)*interval;
  logline(bell,n,0,"start  ");
  while(bell<target){
    long long nb=(long long)(mpz_sizeinbase(n,2)/4); if(nb<(1LL<<16)) nb=1LL<<16;
    if(target-bell<nb) nb=target-bell;
    if(nextck-bell<nb) nb=nextck-bell;
    if(pwB!=nb){ mpz_ui_pow_ui(pw,3,(unsigned long)nb); pwB=nb; }
    mpz_fdiv_qr(q,r,n,pw);
    gbell=bell; adv(r,nb,0);
    if(halted){
      char msg[512]; snprintf(msg,sizeof msg,"HALT at Bell %lld: W = %s, m = %lld (block started at Bell %lld)\n",haltbell,wname[haltW],haltM,bell);
      fputs(msg,stdout); FILE*h=fopen("HALT3.txt","w"); if(h){ fputs(msg,h); fclose(h); } h=fopen("progress3.log","a"); if(h){ fputs(msg,h); fclose(h); }
      return 0; }
    mpz_mul_2exp(n,q,2*(mp_bitcnt_t)nb); mpz_add(n,n,r); bell+=nb;
    if(bell==nextck||bell==target){ if(write_ckpt(bell,n)) return 1; logline(bell,n,now()-t0,bell==target?"target ":"ckpt   "); if(bell==nextck) nextck+=interval; }
  }
  printf("no halt up to Bell %lld; time %.1fs\n",bell,now()-t0);
  return 0; }

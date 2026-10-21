#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const char*Mc="1RB0RB_1LC1RF_0RD0LB_0RA1LE_1LD---_0RB0RC";
int main(int argc,char**argv){ long long n0=atoll(argv[1]); const char*Wd=argv[2]; int mu=atoi(argv[3]);
 int Wt[6][2],D[6][2],N[6][2];
 for(int i=0;i<6;i++)for(int b=0;b<2;b++){const char*x=Mc+7*i+3*b; if(x[0]=='-'){N[i][b]=-1;continue;} Wt[i][b]=x[0]-'0';D[i][b]=x[1]=='R'?1:-1;N[i][b]=x[2]-'A';}
 size_t L=1<<27; unsigned char*t=calloc(L,1); long long o=L/4, k=o;
 const char*pre="011010"; for(int i=0;pre[i];i++) t[k++]=pre[i]-'0';
 for(long long i=0;i<n0;i++) t[k++]=1;
 for(int i=0;Wd[i];i++) t[k++]=Wd[i]-'0';
 for(int u=0;u<mu;u++){ const char*U="01011"; for(int i=0;i<5;i++) t[k++]=U[i]-'0'; }
 long long p=o,lo=o,hi=k-1; int s=3; int bells=0;
 for(long long st=0;;st++){ int b=t[p]; if(N[s][b]<0){ printf("TM HALT after %d further Bells, %lld TM steps, state %c reading %d\n",bells,st,'A'+s,b); return 0; }
   t[p]=Wt[s][b]; p+=D[s][b]; s=N[s][b];
   if(p>hi) hi=p;
   if(p<lo){ lo=p; if(s==3 && t[lo]==0&&t[lo+1]==1&&t[lo+2]==1&&t[lo+3]==0&&t[lo+4]==1&&t[lo+5]==0&&t[lo+6]==1){ long long c=0; while(t[lo+6+c]) c++; if(c>3 && st>0){ bells++; printf("Bell +%d: n=%lld (mod 3 = %lld), step %lld\n",bells,c,c%3,st); fflush(stdout);} } }
   if(p<10||p>(long long)L-10){ printf("tape overflow\n"); return 1; } }
}

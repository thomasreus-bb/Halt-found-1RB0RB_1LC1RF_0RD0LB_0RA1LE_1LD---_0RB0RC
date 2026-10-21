import sys,time
from iter1 import step,split
N=int(sys.argv[1])
n=4; W,m=split('01011'); t0=time.time(); mmin=m; mmax=m; ms=0
for i in range(1,N+1):
    r=step(n,W,m)
    if r is None: print('NO TABLE ENTRY at Bell',i,W,m); break
    if r=='HALT': print('HALT at Bell',i,'word',W,'m',m,'n bits',n.bit_length()); break
    n,W,m,c=r; mmin=min(mmin,m); mmax=max(mmax,m); ms+=m
    if i%(N//10)==0: print('Bell',i,'n bits',n.bit_length(),'W',W,'m',m,'m range',mmin,mmax,'mean m',ms/i,'%.0fs'%(time.time()-t0)); sys.stdout.flush()

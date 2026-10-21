import sys,os
sys.path.insert(0,os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','src'))
from bellrun import run
from iter_table import table as T
U='01011'
def split(t):
    m=0
    while t.endswith(U): t=t[:-5]; m+=1
    return t,m
bad=0; tested=0
for (W,mk,r),v in sorted(T.items()):
    ms=[mk] if mk<3 else [3,5,11]
    for m in ms:
        for a in (41,97,160):
            n=3*a+r
            out=run(n,W+U*m,10**8)
            if out[0]=='HALT': w=('HALT',None,None)
            elif out[0]=='?': w=('?',None,None)
            else:
                W2,m2=split(out[1]); w=(W2,m2-m,3*out[0]-4*n)
            tested+=1
            if w!=v: bad+=1; print('MISMATCH',repr(W),mk,r,'n',n,'m',m,'table',v,'TM',w,flush=True)
print('tested',tested,'mismatches',bad)

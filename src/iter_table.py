import sys,os
# read table/bell_table.txt
table={}
_p=os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','table','bell_table.txt')
for l in open(_p):
    if l.startswith('#') or not l.strip(): continue
    a,b=l.split('->'); W,mk,r=a.split(); W='' if W=='-' else W
    x=b.split()
    table[(W,int(mk),int(r))]=('HALT',None,None) if x[0]=='HALT' else ('' if x[0]=='-' else x[0],int(x[1]),int(x[2]))
U='01011'
def split(t):
    m=0
    while t.endswith(U): t=t[:-5]; m+=1
    return t,m
def step(n,W,m):
    mk=m if m<3 else 3
    v=table.get((W,mk,n%3))
    if v is None: return None
    W2,dm,c=v
    if W2=='HALT': return 'HALT'
    return ((4*n+c)//3, W2, m+dm, c)
if __name__=='__main__':
    W,m=split('0101101011'); n=4  # Bell 1 of the orbit: n=4, tail '01011 01011'? (R=[01 11 01 12] = one unit)
    W,m=split('01011'); seq=[n]
    for i in range(40):
        r=step(n,W,m)
        if r in (None,'HALT'): print('stop',r,i,n,W,m); break
        n,W,m,c=r; seq.append(n)
    print(seq[:32])

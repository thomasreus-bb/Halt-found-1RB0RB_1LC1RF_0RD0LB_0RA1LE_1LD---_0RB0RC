import pickle,random
from bellrun import run
U='01011'
def split(t):
    m=0
    while t.endswith(U): t=t[:-5]; m+=1
    return t,m
def mc(m): return m if m<3 else 3
words={'':1}; queue=[('',mc(m)) for m in range(4)]; seen=set(queue); table={}
while queue and len(seen)<3000:
    W,mk=queue.pop(0); m=mk if mk<3 else 8
    for r in range(3):
        n=90+r
        out=run(n,W+U*m,5*10**7)
        if out[0] in ('HALT','?'): table[(W,mk,r)]=(out[0],None,None); continue
        n2,tail=out; W2,m2=split(tail); c=3*n2-4*n; dm=m2-m
        table[(W,mk,r)]=(W2,dm,c)
        nm=m2 if mk<3 else None
        nxts=[(W2,mc(m2))] if mk<3 else [(W2,3)] + ([(W2,mc(8+dm-k)) for k in []])
        for s in nxts:
            if s not in seen and len(W2)<80: seen.add(s); queue.append(s)
print('states',len(seen),'table',len(table),'queue',len(queue))
pickle.dump(table,open('table1.pkl','wb'))  # the repository keeps the same table as text in table/bell_table.txt
halts=[k for k,v in table.items() if v[0]=='HALT']; unk=[k for k,v in table.items() if v[0]=='?']
print('HALT entries',len(halts),halts[:10]); print('unknown',len(unk),unk[:5])
# check n-independence on a sample
bad=0
for (W,mk,r),v in random.sample(sorted(table.items()),40):
    m=mk if mk<3 else 8
    for n in (150+r,301+((r-301)%3)):
        out=run(n,W+U*m,5*10**7)
        if out[0] in ('HALT','?'): w=(out[0],None,None)
        else:
            W2,m2=split(out[1]); w=(W2,m2-m,3*out[0]-4*n)
        if w!=v: bad+=1; print('MISMATCH',W,mk,r,n,v,w)
print('n-independence mismatches:',bad)

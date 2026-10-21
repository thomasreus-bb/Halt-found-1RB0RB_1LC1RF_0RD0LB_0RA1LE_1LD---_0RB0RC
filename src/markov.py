# Markov estimate: n mod 3 is treated as independent and uniform; chain on the words W (class m >= 3).
from iter_table import table as T
words=sorted({k[0] for k in T if k[1]==3}); idx={w:i for i,w in enumerate(words)}
p=[0.0]*len(words); p[idx['']]=1.0
for it in range(20000):
    q=[0.0]*len(words); h=0.0
    for w,pw in zip(words,p):
        if pw==0: continue
        for r in range(3):
            v=T[(w,3,r)]
            if v[0]=='HALT': h+=pw/3
            else: q[idx[v[0]]]+=pw/3
    s=sum(q); p=[x/s for x in q]
dm=sum(p[idx[w]]*T[(w,3,r)][1]/3 for w in words for r in range(3) if T[(w,3,r)][0]!='HALT')
print('words: %d   halting rate per Bell: %.4g   expected Bells to halt: %.3g   drift of m per Bell: %.4f'%(len(words),h,1/h,dm))
for pr,w in sorted(zip(p,words),reverse=True)[:6]: print('  P(W=%s) = %.4f'%(w or 'empty',pr))

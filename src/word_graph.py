import math,collections,subprocess,sys
T={}
for l in open('table/bell_table.txt'):
    if l.startswith('#') or not l.strip(): continue
    f=l.split(); W=f[0]; mk=int(f[1]); r=int(f[2])
    T[(W,mk,r)]=('HALT',) if f[4]=='HALT' else (f[4],int(f[5]),int(f[6]))
words=sorted({k[0] for k in T}); print(len(words),'words',len(T),'entries')
# stationary distribution of the chain for m >= 3 (as in src/markov.py)
w3=sorted({k[0] for k in T if k[1]==3}); idx={w:i for i,w in enumerate(w3)}
p=[0.0]*len(w3); p[idx['-']]=1.0
for it in range(5000):
    q=[0.0]*len(w3)
    for w,pw in zip(w3,p):
        if pw==0: continue
        for r in range(3):
            v=T[(w,3,r)]
            if v[0]!='HALT': q[idx[v[0]]]+=pw/3
    s=sum(q); p=[x/s for x in q]
P={w:p[idx[w]] for w in w3}
edges=collections.defaultdict(set)
for (W,mk,r),v in T.items(): edges[(W,v[0])].add(mk)
def col(x):  # orange scale by log frequency
    if x<=0: return '#DDDDDD'
    t=max(0.0,min(1.0,(math.log10(x)+7)/7))
    r=255; g=int(235-120*t); b=int(200-200*t); return '#%02X%02X%02X'%(r,g,b)
lines=['digraph W {','  layout=sfdp; overlap=prism; overlap_scaling=-6; splines=false; outputorder=edgesfirst; bgcolor="white";',
 '  node [shape=circle, style=filled, fixedsize=true, label="", color="#B07000", penwidth=0.6];',
 '  edge [color="#00000030", arrowsize=0.35, penwidth=0.6];']
for w in words:
    x=P.get(w,0.0); sz=0.12+0.9*max(0.0,(math.log10(x)+7)/7) if x>0 else 0.12
    inside=('empty' if w=='-' else w) if x>0.02 else ''
    fs=max(7,min(13,int(sz*14)))
    if w in ('00','00010'):
        lines.append('  "%s" [width=%.2f, fillcolor="%s", color="#C0392B", penwidth=2.0, xlabel="%s", fontsize=11, fontname="Helvetica-Bold", fontcolor="#C0392B"];'%(w,sz,col(x),w))
    else:
        lines.append('  "%s" [width=%.2f, fillcolor="%s", label="%s", fontsize=%d, fontname="Helvetica"];'%(w,sz,col(x),inside,fs))
lines.append('  HALT [shape=octagon, width=0.6, fillcolor="#E74C3C", fontcolor="white", label="HALT", fontname="Helvetica-Bold", fontsize=10];')
for (a,b),m in edges.items():
    if a==b: continue
    if b=='HALT': lines.append('  "%s" -> HALT [color="#C0392B", penwidth=2.0, arrowsize=0.7];'%a)
    else: lines.append('  "%s" -> "%s";'%(a,b))
lines.append('}')
open(sys.argv[1],'w').write('\n'.join(lines))

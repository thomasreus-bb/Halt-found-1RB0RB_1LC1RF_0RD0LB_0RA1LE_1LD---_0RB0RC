M='1RB0RB_1LC1RF_0RD0LB_0RA1LE_1LD---_0RB0RC'
T={}
for i,t in enumerate(M.split('_')):
    for b in (0,1):
        x=t[3*b:3*b+3]; T[(i,b)]=None if x[0]=='-' else (int(x[0]),1 if x[1]=='R' else -1,ord(x[2])-65)
def rle(tape,lo,hi):
    out=[];i=lo
    while i<=hi:
        j=i
        while j<=hi and tape.get(j,0)==tape.get(i,0): j+=1
        out.append((tape.get(i,0),j-i)); i=j
    return out
def run(n,tail,maxsteps=2*10**7):
    s='011010'+'1'*n+tail
    tape={i:int(c) for i,c in enumerate(s)}; lo=0; hi=len(s)-1; p=0; st=3
    for k in range(maxsteps):
        b=tape.get(p,0); tr=T[(st,b)]
        if tr is None: return ('HALT',k)
        tape[p]=tr[0]; p+=tr[1]; st=tr[2]
        if p>hi: hi=p
        if p<lo:
            lo=p
            if st==3:
                r=rle(tape,lo,hi)
                # expect 0 11 0 1 0 1^n' tail
                if len(r)>=6 and r[:5]==[(0,1),(1,2),(0,1),(1,1),(0,1)] and r[5][0]==1 and r[5][1]>3:
                    nn=r[5][1]; rest=''.join(str(c)*l for c,l in r[6:])
                    return (nn,rest)
    return ('?',None)
if __name__=='__main__':
    import sys
    # first: verify with real orbit value n=212, tail observed
    print(run(212,'0101101011'+'0101101011'))

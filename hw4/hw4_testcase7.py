def f(m,n):
    if m==0:
        return n+1
    else:
        return f(m-1, n+1)
f(3,4)

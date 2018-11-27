def f():
    a = 1
    b = 10
    c = a + b
    d = c * a -b
    if(c == a):
        m = 100
    else:
        m = 50
    return m

x = f()

if(x!=1000):
    print ("value is not 1000, it is =",x)
else:
    print ("value is 1000")
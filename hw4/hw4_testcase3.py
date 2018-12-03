def f():
    print("Recursive function")
    return f()

def f1(n):
    print("here")
    if n==1:
        return 1
    else:
        return n * f1(n - 1)

f()
f1(5)

print("That's it")

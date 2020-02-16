import random
import string

for i in range(3):
    filename = "file" + str(i)
    with open(filename,'w') as fp:
        wstr =''.join(random.choices(string.ascii_lowercase, k=10))
        print(wstr)
        fp.write(wstr+"\n")

int1 = random.randint(1,42)
int2 = random.randint(1,42)
product = int1*int2

print(int1)
print(int2)
print(product)

Files in this directory demonstrate bash shell scripting by performing basic operations on tab delimited matrix files.

Usage:
```
./matrix dims m1    #returns rows, cols
./matrix add m1 m1
./matrix mean m1    #outputs column wise means
./matrix transpose m1
./matrix multiply m1 m2
```

Input can also be read from stdin.

```
cat m2 | ./matrix dims
```

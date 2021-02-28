# Assumes you called:
# make &> make.log
import sys

token = "undefined reference to `"
D = {}
f = open(sys.argv[1],'r')
for line in f.readlines():
    i = line.find(token)
    if i!=-1:
        x = line[i+len(token):-2]
        try:
            D[x] = D[x] + 1
        except KeyError:
            D[x] = 1
f.close()

for x in sorted(D, key=D.get, reverse=True):
    print(D[x],x)
# cube has 8 corners
from collections import defaultdict

ULB = 0
URB = 1
URF = 2
ULF = 3
DLF = 4
DLB = 5
DRB = 6
DRF = 7

cube = [(-1,1,-1),(1,1,-1),(1,1,1),(-1,1,1),(-1,-1,1),(-1,-1,-1),(1,-1,-1),(1,-1,1)]
manhatten = defaultdict(dict)

def dist(a, b):
    return (abs(a[0]-b[0]) + abs(a[1]-b[1]) + abs(a[2]-b[2])) // 2

for i in range(8):
    for j in range(8):
        manhatten[i][j] = dist(cube[i], cube[j])

s = "0b"
for i in range(8):
    for j in range(8):
        s += f"{manhatten[i][j]&1}"
print(hex(int(s, 2)))

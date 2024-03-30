#!/usr/bin/env python3

# cube as defined by Korf
corners = ['ULB', 'URB', 'URF', 'ULF', 'DLF', 'DLB', 'DRB', 'DRF']
edges = ['UB', 'UR', 'UF', 'UL', 'FR', 'FL', 'BL', 'BR', 'DF', 'DL', 'DB', 'DR']
centers = ['RED', 'GREEN', 'BLUE', 'WHITE', 'ORANGE', 'YELLOW']

# faces
front = ['ULF', 'UF', 'URF', 'FL', 'WHITE', 'FR', 'DLF', 'DF', 'DRF']
up = ['ULB', 'UB', 'URB', 'UL', 'RED', 'UR', 'ULF', 'UF', 'URF']
left = ['ULB', 'UL', 'ULF', 'BL', 'BLUE', 'FL', 'DLB', 'DL', 'DLF']
right = ['URF', 'UR', 'URB', 'FR', 'GREEN', 'BR', 'DRF', 'DR', 'DRB']
back = ['URB', 'UB', 'ULB', 'BR', 'YELLOW', 'BL', 'DRB', 'DB', 'DLB']
down = ['DLF', 'DF', 'DRF', 'DL', 'ORANGE', 'DR', 'DLB', 'DB', 'DRB']

M = edges + corners + centers

def indices(M, face, char):
    l = []
    for f in face:
        i = M.index(f)
        if i < 12:
            j = f.index(char)
            l.append(i*2+j)
        elif i < 20:
            j = f.index(char)
            l.append(24 + (i-12) * 3 + j)
        else:
            l.append((i - 20) + 48)
    return l
    
# print all indices to form a 2d mapping of the 3d cube
#
#           R R R
#           R R R
#           R R R
#
#    B B B  W W W  G G G  Y Y Y
#    B B B  W W W  G G G  Y Y Y
#    B B B  W W W  G G G  Y Y Y
#
#           O O O
#           O O O
#           O O O

F = indices(M, front, 'F')
U = indices(M, up, 'U')
D = indices(M, down, 'D')
L = indices(M, left, 'L')
R = indices(M, right, 'R')
B = indices(M, back, 'B')

mapping = \
    U + \
    L[:3] + F[:3] + R[:3] + B[:3] + \
    L[3:6] + F[3:6] + R[3:6] + B[3:6] + \
    L[6:] + F[6:] + R[6:] + B[6:] + \
    D

print(",".join(map(str, mapping)))

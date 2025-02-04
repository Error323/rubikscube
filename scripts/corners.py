#!/usr/bin/env python3

import copy
import random
import sys

ULB = 0
URB = 1
URF = 2
ULF = 3
DLF = 4
DLB = 5
DRB = 6
DRF = 7


# YXZ format
M = (
    # even corners
    {0:('012', '021'), 
     1:('120', '102'), 
     2:('201', '210')},
    # uneven corners
    {0:('012', '021'), 
     1:('201', '210'), 
     2:('120', '102')},
)

moves = "RUFLDB"
colors  = ['WOB', 'WRB', 'WRG', 'WOG', 'YOG', 'YOB', 'YRB', 'YRG']
corners = ['ULB', 'URB', 'URF', 'ULF', 'DLF', 'DLB', 'DRB', 'DRF']
mapping = [0,3,9,6,1,10,11,8,7,4,5,2,16,13,14,23,22,19,20,17,12,21,15,18]

y = 0
x = 1
z = 2

class Cubie:
    def __init__(self, idx, ori):
        self.idx = idx
        self.ori = ori
        self.flp = 0 # manhatten distance modulo 2
        self.colors = list(colors[idx])

    def indices(self):
        return "".join(map(lambda x: str(colors[self.idx].index(x)), self.colors))

    def __repr__(self):
        I = self.indices()
        return f"({corners[self.idx]}, {self.ori}, {self.flp}, {I})"


class Cube:
    def __init__(self):
        self.corner = []
        for i in range(len(corners)):
            self.corner.append(Cubie(i, 0))


    def u(self):
        tmp = copy.deepcopy(self.corner[ULF])
        self.corner[ULF] = self.corner[URF]
        self.corner[URF] = self.corner[URB]
        self.corner[URB] = self.corner[ULB]
        self.corner[ULB] = tmp
        for c in [ULF, URF, URB, ULB]:
            self.corner[c].colors[x], self.corner[c].colors[z] = self.corner[c].colors[z], self.corner[c].colors[x]
            self.corner[c].flp ^= 1

    def d(self):
        tmp = copy.deepcopy(self.corner[DLB])
        self.corner[DLB] = self.corner[DRB]
        self.corner[DRB] = self.corner[DRF]
        self.corner[DRF] = self.corner[DLF]
        self.corner[DLF] = tmp
        for c in [DLF, DRF, DRB, DLB]:
            self.corner[c].colors[x], self.corner[c].colors[z] = self.corner[c].colors[z], self.corner[c].colors[x]
            self.corner[c].flp ^= 1


    def f(self):
        tmp = copy.deepcopy(self.corner[ULF])
        self.corner[ULF] = self.corner[DLF]
        self.corner[DLF] = self.corner[DRF]
        self.corner[DRF] = self.corner[URF]
        self.corner[URF] = tmp
        self._update(ULF, 1)
        self._update(URF, 2)
        self._update(DRF, 1)
        self._update(DLF, 2)
        for c in [ULF, URF, DRF, DLF]:
            self.corner[c].colors[x], self.corner[c].colors[y] = self.corner[c].colors[y], self.corner[c].colors[x]
            self.corner[c].flp ^= 1


    def b(self):
        tmp = copy.deepcopy(self.corner[ULB])
        self.corner[ULB] = self.corner[URB]
        self.corner[URB] = self.corner[DRB]
        self.corner[DRB] = self.corner[DLB]
        self.corner[DLB] = tmp
        self._update(ULB, 2)
        self._update(URB, 1)
        self._update(DRB, 2)
        self._update(DLB, 1)
        for c in [ULB, URB, DRB, DLB]:
            self.corner[c].colors[x], self.corner[c].colors[y] = self.corner[c].colors[y], self.corner[c].colors[x]
            self.corner[c].flp ^= 1


    def r(self):
        tmp = copy.deepcopy(self.corner[DRB])
        self.corner[DRB] = self.corner[URB]
        self.corner[URB] = self.corner[URF]
        self.corner[URF] = self.corner[DRF]
        self.corner[DRF] = tmp
        self._update(DRB, 1)
        self._update(URB, 2)
        self._update(URF, 1)
        self._update(DRF, 2)
        for c in [DRB, URB, URF, DRF]:
            self.corner[c].colors[y], self.corner[c].colors[z] = self.corner[c].colors[z], self.corner[c].colors[y]
            self.corner[c].flp ^= 1


    def l(self):
        tmp = copy.deepcopy(self.corner[DLB])
        self.corner[DLB] = self.corner[DLF]
        self.corner[DLF] = self.corner[ULF]
        self.corner[ULF] = self.corner[ULB]
        self.corner[ULB] = tmp
        self._update(DLB, 2)
        self._update(DLF, 1)
        self._update(ULF, 2)
        self._update(ULB, 1)
        for c in [DLB, DLF, ULF, ULB]:
            self.corner[c].colors[y], self.corner[c].colors[z] = self.corner[c].colors[z], self.corner[c].colors[y]
            self.corner[c].flp ^= 1


    def _update(self, corner, n):
        self.corner[corner].ori += n
        self.corner[corner].ori %= 3


    def _colors(self, corner):
        cubie = self.corner[corner]
        I = M[cubie.idx & 1][cubie.ori][cubie.flp]
        i0, i1, i2 = list(map(lambda x: int(x), I))
        return colors[cubie.idx][i0], colors[cubie.idx][i1], colors[cubie.idx][i2]


    def __repr__(self):
        s = "\n"
        faces = []
        for i in range(8):
            cubie = self.corner[i]
            a,b,c = cubie.colors
            d,e,f = cube._colors(i)
            if a!=d or b!=e or c!=f:
                print(f" {colors[cubie.idx]} {cubie} {a}{b}{c} != {d}{e}{f}")
                sys.exit(1)
            faces.append(a)
            faces.append(b)
            faces.append(c)

        for i in range(2):
            s += 5 * " "
            for j in range(2):
                s += f"{faces[mapping[i * 2 + j]]} "
            s += "\n"

        for r in range(2):
            for i in range(4):
                for j in range(2):
                    s += f"{faces[mapping[4 + r * 8 + i * 2 + j]]} "
                s += " "
            s += "\n"

        for i in range(2):
            s += 5 * " "
            for j in range(2):
                s += f"{faces[mapping[20 + i * 2 + j]]} "
            s += "\n"

        return s


def get_op(cube, move):
    if move == 'R':
        return cube.r
    elif move == 'U':
        return cube.u
    elif move == 'F':
        return cube.f
    elif move == 'L':
        return cube.l
    elif move == 'D':
        return cube.d
    elif move == 'B':
        return cube.b
    else:
        return None
    




if __name__ == "__main__":

    V = {}
    for i in range(8):
        V[i] = {}
        for j in range(3):
            V[i][j] = []

    for i in range(100):
        cube = Cube()
        for _ in range(20):
            move = random.choice(moves)
            get_op(cube, move)()
            for c in cube.corner:
                I = c.indices()
                if I not in V[c.idx][c.ori]:
                    if c.flp == 0:
                        V[c.idx][c.ori].insert(0, I)
                    else:
                        V[c.idx][c.ori].append(I)

    for k in V:
        print(f"\n{k} {corners[k]}")
        for kk, vv in V[k].items():
            print(f"  {kk} {vv}")

    if len(sys.argv) > 1:
        cube = Cube()
        for m in sys.argv[1]:
            assert(m in moves)
            get_op(cube, m)()
        print(cube)

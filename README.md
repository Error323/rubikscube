Problem space:
Rubiks cube 3x3x3
Cubie 1x1x1
Manipulatable cubies: 20 (8 corners 3 faces each, 12 edges 2 faces each)
corner cubies move along corners
edge cubies move along edges

state of a cube can be specified uniquely by:
- pos and orientation of 8 corner cubies (3 orientations)
- pos and orientation of 12 edge cubies (2 orientations)

What is the most efficient encoding? also taking actions into account
- corner cubie: 3 + 3 bits (position + orientation)
- edge cubie: 4 + 1 bits
- 6 * 8 bits + 5 * 12 bits = 108 

108 / 8 = 13.5 bytes


Action space:
L,R,U,D,F,B
L',R',U',D',F',B'
2L, 2R, 2U, 2D, 2F, 2B
After the first move we can reduce the actions space from 18 to ~13.4, see genmoves.py

Heuristic function:
Pattern databases precomputed for sub solutions

Search algo:
We can try true A*, given that the heuristic from the pattern database is consistent i.e. h(x) <= g(x, y) + h(y) for every edge (x, y), we can omit the g-cost check and therefore do not need a hash table. We keep parents of nodes as pointers and use a b-heap that optimizes for page tables.

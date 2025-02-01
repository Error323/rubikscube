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

Cost function:
Pattern databases precomputed from solved state

Search algo:
IDA* - Iterative Deepening A*
This is a depth first search
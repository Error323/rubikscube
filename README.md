Problem space:
Rubiks cube 3x3x3
Cubie 1x1x1
Manipulatable cubies: 20 (8 corners 3 faces each, 12 edges 2 faces each)
corner cubies move along corners
edge cubies move along edges

state of a cube can be specified uniquely by:
 - pos and orientation of 8 corner cubies (3 orientations)
 - pos and orientation of 12 edge cubies (2 orientations)
encoded using an array of 20 elements

What is the most efficient encoding? also taking actions into account
position x,y,z in {0,1,2}, 6 bits total
orientation corner cubies in {0,1,2} 2 bits
orientation edge cubies in {0,1} 1 bit

uint8_t cube[20];

Action space:
L,R,U,D,F,B
L',R',U',D',F',B'
2L, 2R, 2U, 2D, 2F, 2B

Cost function:
Manhatten distance of edge cubies

Search algo:
IDA* - Iterative Deepening A*



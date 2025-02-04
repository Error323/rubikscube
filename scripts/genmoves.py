#!/usr/bin/env python

moves = ["R", "2R", "R'", "L", "2L", "L'", "U", "2U", "U'", "D", "2D", "D'", "F", "2F", "F'", "B", "2B", "B'"]
oppossite = [("L", "R"), ("D", "U"), ("B", "F")]

all = 0b111111111111111111
validbits = [all]
for i, move in enumerate(moves):
    char = move[0]
    if len(move) == 2:
        if move[0] == "2":
            char = move[1]
    
    valid = all
    opp = [x for c, x in oppossite if c == char]
    if len(opp) == 1:
        indices = [j for j, m in enumerate(moves) if opp[0] in m]
        for mid in indices:
            valid ^= 1 << mid

    indices = [j for j, m in enumerate(moves) if char in m]
    for mid in indices:
        valid ^= 1 << mid
    validbits.append(valid)

print("static const u32 kValidMoves[] = {")
print(",\n".join(map(lambda x: f"    {x:018b}", validbits)))
print("}")



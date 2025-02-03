/// Allows us to compare movegenerator with Korf's paper
///
/// depth |            nodes
/// ------+-----------------------------
/// 1     |                           18
/// 2     |                          243
/// 3     |                        3,240
/// 4     |                       43,254
/// 5     |                      577,368
/// 6     |                    7,706,988
/// 7     |                  102,876,480
/// 8     |                1,373,243,544
/// 9     |               18,330,699,168
/// 10    |              244,686,773,808
/// 11    |            3,266,193,870,720
/// 12    |           43,598,688,377,184
/// 13    |          581,975,750,199,168
/// 14    |        7,768,485,393,179,328
/// 15    |      103,697,388,221,736,960
/// 16    |    1,384,201,395,738,071,424
/// 17    |   18,476,969,736,848,122,368
/// 18    |  246,639,261,965,462,754,048
internal u64 Dfs(Cube cube, s32 depth) {
    if (depth == 0) {
        return 1;
    }

    u32 valid = kValidMoves[cube.GetLastMoveIndex()];
    u64 sum = 0;
    Cube tmp = cube;
    while (valid) {
        s32 move = __builtin_ffs(valid) - 1;
        valid &= valid - 1;
        ApplyMove(cube, move);
        sum += Dfs(cube, depth - 1);
        cube = tmp;
    }

    return sum;
}
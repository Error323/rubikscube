//! A Rubik's Cube model (3x3x3) based on Richard E. Korf's paper.
//!
//!   Finding Optimal Solutions to Rubik's Cube
//!          Using Pattern Databases
//!
//! A cubie is an 1x1x1 cube on the rubiks cube with an index
//! and orientation. In a solved state the cube is facing you with
//! the white side in front and red side on top.
//!
//!                    Up
//!
//!                   R R R
//!                   R R R
//!                   R R R 
//!             B B B W W W G G G Y Y Y
//!       Left  B B B W W W G G G Y Y Y  Back
//!             B B B W W W G G G Y Y Y
//!                   O O O
//!                   O O O
//!                   O O O
//!                     
//!                   Down
//!
//! Edge indices (12):
//!
//!     0  1  2  3  4  5  6  7  8  9  10 11
//! pos UB UR UF UL FR FL BL BR DF DL DB DR
//! ori RY RG RW RB WG WB YB YG OW OB OY OG
//!
//!
//! Corner indices (8):
//!
//!     0   1   2   3   4   5   6   7
//! pos ULB URB URF ULF DLF DLB DRB DRF
//! ori RBY RGY RGW RBW OBW OBY OGY OGW
//!
//! U=Up, B=Back, L=Left, R=Right, F=Front, D=Down
//! R=Red, G=Green, B=Blue, W=White, Y=Yellow, O=Orange


const Edge = enum(i8) { UB, UR, UF, UL, FR, FL, BL, BR, DF, DL, DB, DR };
const Corner = enum(i8) { ULB, URB, URF, ULF, DLF, DLB, DRB, DRF };
const Color = enum(i8) { RED, GREEN, BLUE, WHITE, ORANGE, YELLOW };
const Cubie = struct {
    index: i8,
    orientation: i8,
};

const Cube = struct {
    edge: [12]Cubie,
    corner: [8]Cubie,
};

const print = @import("std").debug.print;
const swap = @import("std").mem.swap;

pub fn init() Cube {
    var cube: Cube = undefined;

    for (&cube.edge, 0..) |*edge, i| {
        edge.index = @as(i8, @intCast(i));
        edge.orientation = 0;
    }

    for (&cube.corner, 0..) |*corner, i| {
        corner.index = @as(i8, @intCast(i));
        corner.orientation = 0;
    }

    return cube;
}

pub fn edgeColors(cube: Cube, edge: Edge) struct { Color, Color } {
    const cubie: Cubie = cube.edge[@as(usize, @intCast(@intFromEnum(edge)))];

    const index: Edge = @enumFromInt(cubie.index);
    var colors = switch (index) {
        Edge.UB => .{ Color.RED, Color.YELLOW },
        Edge.UR => .{ Color.RED, Color.GREEN },
        Edge.UF => .{ Color.RED, Color.WHITE },
        Edge.UL => .{ Color.RED, Color.BLUE },
        Edge.FR => .{ Color.WHITE, Color.GREEN },
        Edge.FL => .{ Color.WHITE, Color.BLUE },
        Edge.BL => .{ Color.YELLOW, Color.BLUE },
        Edge.BR => .{ Color.YELLOW, Color.GREEN },
        Edge.DF => .{ Color.ORANGE, Color.WHITE },
        Edge.DL => .{ Color.ORANGE, Color.BLUE },
        Edge.DB => .{ Color.ORANGE, Color.YELLOW },
        Edge.DR => .{ Color.ORANGE, Color.GREEN },
    };

    if (cubie.orientation == 1) {
        swap(Color, &colors[0], &colors[1]);
    }

    return colors;
}

pub fn cornerColors(cube: Cube, corner: Corner) struct { Color, Color, Color } {
    const cubie: Cubie = cube.corner[@as(usize, @intCast(@intFromEnum(corner)))];

    var color: struct { Color, Color, Color } = undefined;


    const index: Corner = @enumFromInt(corner.index);
    switch (index) {
    }

    return color;
}


pub fn f(cube: Cube) Cube {
    var tmp: Cubie = cube.corner[Corner.ULF];
    cube.corner[Corner.ULF] = cube.corner[Corner.DLF];
    cube.corner[Corner.URF] = tmp;
    return cube;
}

pub fn render(cube: Cube) void {
    // gather top
    print("{}", .{cube.corner[0].index});
    const c = edgeColors(cube, Edge.DR);
    print("{} {}", .{ c[0], c[1] });
    // gather left, front, right, back
    // gather bottom
}

pub fn main() void {
    var cube = init();
    render(cube);
}

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
//!
//!            B B B  W W W  G G G  Y Y Y
//!      Left  B B B  W W W  G G G  Y Y Y  Back
//!            B B B  W W W  G G G  Y Y Y
//!
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

const print = @import("std").debug.print;
const swap = @import("std").mem.swap;

const Edge = enum(usize) { UB, UR, UF, UL, FR, FL, BL, BR, DF, DL, DB, DR };
const Corner = enum(usize) { ULB, URB, URF, ULF, DLF, DLB, DRB, DRF };
const Color = enum(usize) { RED, GREEN, BLUE, WHITE, ORANGE, YELLOW };

const Cubie = struct {
    index: usize,
    orientation: usize,
};

const Cube = struct {
    edge: [12]Cubie,
    corner: [8]Cubie,

    pub fn init() Cube {
        var cube: Cube = undefined;

        for (&cube.edge, 0..) |*edge, i| {
            edge.index = i;
            edge.orientation = 0;
        }

        for (&cube.corner, 0..) |*corner, i| {
            corner.index = i;
            corner.orientation = 0;
        }

        return cube;
    }

    pub fn edgeColors(self: Cube, edge: Edge) [2]Color {
        const cubie: Cubie = self.edge[@intFromEnum(edge)];

        const edgeColorMapping = [12][2]Color{
            [2]Color{ Color.RED, Color.YELLOW },
            [2]Color{ Color.RED, Color.GREEN },
            [2]Color{ Color.RED, Color.WHITE },
            [2]Color{ Color.RED, Color.BLUE },
            [2]Color{ Color.WHITE, Color.GREEN },
            [2]Color{ Color.WHITE, Color.BLUE },
            [2]Color{ Color.YELLOW, Color.BLUE },
            [2]Color{ Color.YELLOW, Color.GREEN },
            [2]Color{ Color.ORANGE, Color.WHITE },
            [2]Color{ Color.ORANGE, Color.BLUE },
            [2]Color{ Color.ORANGE, Color.YELLOW },
            [2]Color{ Color.ORANGE, Color.GREEN },
        };

        var colors = edgeColorMapping[cubie.index];

        if (cubie.orientation == 1) {
            swap(Color, &colors[0], &colors[1]);
        }

        return colors;
    }

    pub fn cornerColors(self: Cube, corner: Corner) [3]Color {
        const cubie: Cubie = self.corner[@intFromEnum(corner)];

        const cornerColorMapping = [8][3]Color{
            [3]Color{ Color.RED, Color.BLUE, Color.YELLOW },
            [3]Color{ Color.RED, Color.GREEN, Color.YELLOW },
            [3]Color{ Color.RED, Color.GREEN, Color.WHITE },
            [3]Color{ Color.RED, Color.BLUE, Color.WHITE },
            [3]Color{ Color.ORANGE, Color.BLUE, Color.WHITE },
            [3]Color{ Color.ORANGE, Color.BLUE, Color.YELLOW },
            [3]Color{ Color.ORANGE, Color.GREEN, Color.YELLOW },
            [3]Color{ Color.ORANGE, Color.GREEN, Color.WHITE },
        };

        var colors = cornerColorMapping[cubie.index];

        switch (cubie.orientation) {
            // Nothing todo
            0 => {},
            // Rotate clockwise
            1 => {
                const tmp = colors[0];
                colors[0] = colors[1];
                colors[1] = colors[2];
                colors[2] = tmp;
            },
            // Rotate counter clockwise
            2 => {
                const tmp = colors[2];
                colors[2] = colors[1];
                colors[1] = colors[0];
                colors[0] = tmp;
            },
            else => print("Invalid orientation {}", .{cubie.orientation}),
        }
        return colors;
    }

    pub fn str(self: Cube) void {
        const C = [_]u8{ 'R', 'G', 'B', 'W', 'O', 'Y' };
        var faces: [54]u8 = undefined;

        for (0..12) |i| {
            const colors = self.edgeColors(@enumFromInt(i));
            faces[i * 2 + 0] = C[@intFromEnum(colors[0])];
            faces[i * 2 + 1] = C[@intFromEnum(colors[1])];
        }

        for (0..8) |i| {
            const colors = self.cornerColors(@enumFromInt(i));
            faces[24 + i * 3 + 0] = C[@intFromEnum(colors[0])];
            faces[24 + i * 3 + 1] = C[@intFromEnum(colors[1])];
            faces[24 + i * 3 + 2] = C[@intFromEnum(colors[2])];
        }

        for (0..6) |i| {
            faces[48 + i] = C[i];
        }

        // 2d mapping of the cube, 54 faces
        //
        //           R R R
        //           R R R
        //           R R R
        //
        //    B B B  W W W  G G G  Y Y Y
        //    B B B  W W W  G G G  Y Y Y
        //    B B B  W W W  G G G  Y Y Y
        //
        //           O O O
        //           O O O
        //           O O O
        //
        const map = [54]Cubie{
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
            Cubie{ .index = 0, .orientation = 0 },
        };

        for (map) |c| {
            print("{}\n", .{c});
        }
    }
};

// Rotate front clockwise
//pub fn f(cube: *Cube) void {
//}

pub fn main() void {
    var cube = Cube.init();
    cube.str();
}

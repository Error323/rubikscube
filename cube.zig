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
//! 3d  YXZ YXZ YXZ YXZ YXZ YXZ YXZ YXZ
//!
//! Orientations (3):
//!
//!           YXZ DLF
//! Rz(YXZ) = XYZ LDF
//! Ry(XYZ) = ZYX FDL
//! Rx(ZYX) = YZX DFL
//! Rz(YZX) = XZY LFD
//! Ry(XZY) = ZXY FLD
//!
//! U=Up, B=Back, L=Left, R=Right, F=Front, D=Down
//! R=Red, G=Green, B=Blue, W=White, Y=Yellow, O=Orange

const print = @import("std").debug.print;
const swap = @import("std").mem.swap;

const Edge = enum(usize) {
    UB,
    UR,
    UF,
    UL,
    FR,
    FL,
    BL,
    BR,
    DF,
    DL,
    DB,
    DR,
    fn id(self: Edge) usize {
        return @intFromEnum(self);
    }
};

const Corner = enum(usize) {
    ULB,
    URB,
    URF,
    ULF,
    DLF,
    DLB,
    DRB,
    DRF,
    fn id(self: Corner) usize {
        return @intFromEnum(self);
    }
};
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

    /// Rotate the front face clockwise 90 degrees
    pub fn f(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[Corner.ULF.id()];
        self.corner[Corner.ULF.id()] = self.corner[Corner.DLF.id()];
        self.corner[Corner.DLF.id()] = self.corner[Corner.DRF.id()];
        self.corner[Corner.DRF.id()] = self.corner[Corner.URF.id()];
        self.corner[Corner.URF.id()] = tmp;

        self.corner[Corner.ULF.id()].orientation += 2;
        self.corner[Corner.ULF.id()].orientation %= 3;
        self.corner[Corner.URF.id()].orientation += 1;
        self.corner[Corner.URF.id()].orientation %= 3;
        self.corner[Corner.DRF.id()].orientation += 2;
        self.corner[Corner.DRF.id()].orientation %= 3;
        self.corner[Corner.DLF.id()].orientation += 1;
        self.corner[Corner.DLF.id()].orientation %= 3;

        tmp = self.edge[Edge.UF.id()];
        self.edge[Edge.UF.id()] = self.edge[Edge.FL.id()];
        self.edge[Edge.FL.id()] = self.edge[Edge.DF.id()];
        self.edge[Edge.DF.id()] = self.edge[Edge.FR.id()];
        self.edge[Edge.FR.id()] = tmp;

        self.edge[Edge.UF.id()].orientation ^= 1;
        self.edge[Edge.FL.id()].orientation ^= 1;
        self.edge[Edge.DF.id()].orientation ^= 1;
        self.edge[Edge.FR.id()].orientation ^= 1;
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
        const cubie: Cubie = self.corner[corner.id()];

        var o0: usize = 0;
        var o1: usize = 1;
        var o2: usize = 2;

        if (cubie.orientation == 0) {
            if ((cubie.index + corner.id()) % 2 == 1) {
                swap(usize, &o1, &o2);
            }
        }
        if (cubie.orientation == 1) {
            if ((cubie.index + corner.id()) % 2 == 1) {
                swap(usize, &o0, &o1);
            }
        } else if (cubie.orientation == 2) {
            o0 = 2;
            o1 = 0;
            o2 = 1;
            if ((cubie.index + corner.id()) % 2 == 1) {
                swap(usize, &o0, &o2);
            }
        }

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

        return [3]Color{ colors[o0], colors[o1], colors[o2] };
    }

    pub fn render(self: Cube) void {
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

        // 2d mapping of the cube, 54 faces, see map.py
        const map = [54]usize{ 24, 0, 27, 6, 48, 2, 33, 4, 30, 25, 7, 34, 35, 5, 32, 31, 3, 28, 29, 1, 26, 13, 50, 11, 10, 51, 8, 9, 49, 15, 14, 53, 12, 40, 19, 37, 38, 17, 47, 46, 23, 43, 44, 21, 41, 36, 16, 45, 18, 52, 22, 39, 20, 42 };

        print("\n", .{});
        for (0..3) |i| {
            print("        ", .{});
            for (0..3) |j| {
                print("{c} ", .{faces[map[i * 3 + j]]});
            }
            print("\n", .{});
        }
        print("\n", .{});
        for (0..3) |i| {
            print(" ", .{});
            for (0..12) |j| {
                if (j % 3 == 0 and j > 0) {
                    print(" {c} ", .{faces[map[9 + i * 12 + j]]});
                } else {
                    print("{c} ", .{faces[map[9 + i * 12 + j]]});
                }
            }
            print("\n", .{});
        }
        print("\n", .{});
        for (0..3) |i| {
            print("        ", .{});
            for (0..3) |j| {
                print("{c} ", .{faces[map[9 + 3 * 12 + i * 3 + j]]});
            }
            print("\n", .{});
        }
        print("\n", .{});
    }
};

pub fn main() void {
    var cube = Cube.init();
    for (0..4) |_| {
        cube.f();
        cube.render();
    }
}

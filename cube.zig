//! A Rubik's Cube model (3x3x3) based on Richard E. Korf's paper.
//!
//!   Finding Optimal Solutions to Rubik's Cube
//!          Using Pattern Databases
//!
//! A cubie is an 1x1x1 cube on the rubiks cube with an index
//! and orientation. In a solved state the cube is facing you with
//! the orange side in front and yellow side on top. equal to
//! https://ruwix.com/online-rubiks-cube-solver-program/
//!
//!                    Up
//!
//!                   Y Y Y
//!                   Y Y Y
//!                   Y Y Y
//!
//!            G G G  O O O  B B B  R R R
//!      Left  G G G  O O O  B B B  R R R  Back
//!            G G G  O O O  B B B  R R R
//!
//!                   W W W
//!                   W W W
//!                   W W W
//!
//!                   Down
//!
//! Edge indices (12):
//!
//!       0  1  2  3  4  5  6  7  8  9  10 11
//! pos   UB UR UF UL FR FL BL BR DF DL DB DR
//! color RY RG RW RB WG WB YB YG OW OB OY OG
//!
//!
//! Corner indices (8):
//!
//!       0   1   2   3   4   5   6   7
//! pos   ULB URB URF ULF DLF DLB DRB DRF
//! color RBY RGY RGW RBW OBW OBY OGY OGW
//! world YXZ YXZ YXZ YXZ YXZ YXZ YXZ YXZ
//!
//! U=Up, B=Back, L=Left, R=Right, F=Front, D=Down
//! R=Red, G=Green, B=Blue, W=White, Y=Yellow, O=Orange

const print = @import("std").debug.print;
const swap = @import("std").mem.swap;
const std = @import("std");

/// edges
const UB: usize = 0;
const UR: usize = 1;
const UF: usize = 2;
const UL: usize = 3;
const FR: usize = 4;
const FL: usize = 5;
const BL: usize = 6;
const BR: usize = 7;
const DF: usize = 8;
const DL: usize = 9;
const DB: usize = 10;
const DR: usize = 11;

/// corners
const ULB: usize = 0;
const URB: usize = 1;
const URF: usize = 2;
const ULF: usize = 3;
const DLF: usize = 4;
const DLB: usize = 5;
const DRB: usize = 6;
const DRF: usize = 7;

/// colors
const RED: usize = 0;
const GREEN: usize = 1;
const BLUE: usize = 2;
const WHITE: usize = 3;
const ORANGE: usize = 4;
const YELLOW: usize = 5;

/// color orientations, see corner.py
const CO = [2][3][2][3]usize{ [3][2][3]usize{ [2][3]usize{ [_]usize{ 0, 1, 2 }, [_]usize{ 0, 2, 1 } }, [2][3]usize{ [_]usize{ 1, 2, 0 }, [_]usize{ 1, 0, 2 } }, [2][3]usize{ [_]usize{ 2, 0, 1 }, [_]usize{ 2, 1, 0 } } }, [3][2][3]usize{ [2][3]usize{ [3]usize{ 0, 1, 2 }, [_]usize{ 0, 2, 1 } }, [2][3]usize{ [3]usize{ 2, 0, 1 }, [3]usize{ 2, 1, 0 } }, [2][3]usize{ [3]usize{ 1, 2, 0 }, [3]usize{ 1, 0, 2 } } } };

const Cubie = packed struct {
    index: u4,
    orientation: u3,
    // only used for corners, could also be achieved by computing the manhatten
    // distance modulo 2 from its original position
    flipped: u1,

    pub fn flip(self: *Cubie) void {
        self.flipped ^= 1;
    }

    pub fn update(self: *Cubie, n: u8) void {
        self.flipped ^= 1;
        self.orientation += @intCast(n);
        self.orientation %= 3;
    }
};

const Cube = struct {
    edge: [12]Cubie,
    corner: [8]Cubie,

    pub fn init() Cube {
        var cube: Cube = undefined;

        // edges
        for (0..12) |i| {
            cube.edge[i].index = @intCast(i);
            cube.edge[i].orientation = 0;
        }

        // corners
        for (0..8) |i| {
            cube.corner[i].index = @intCast(i);
            cube.corner[i].orientation = 0;
            cube.corner[i].flipped = 0;
        }

        return cube;
    }

    /// Rotate the up face clockwise 90 degrees
    pub fn u(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[ULF];
        self.corner[ULF] = self.corner[URF];
        self.corner[URF] = self.corner[URB];
        self.corner[URB] = self.corner[ULB];
        self.corner[ULB] = tmp;

        self.corner[ULF].flip();
        self.corner[URF].flip();
        self.corner[URB].flip();
        self.corner[ULB].flip();

        tmp = self.edge[UL];
        self.edge[UL] = self.edge[UF];
        self.edge[UF] = self.edge[UR];
        self.edge[UR] = self.edge[UB];
        self.edge[UB] = tmp;
    }

    /// Rotate the down face clockwise 90 degrees
    pub fn d(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[DLB];
        self.corner[DLB] = self.corner[DRB];
        self.corner[DRB] = self.corner[DRF];
        self.corner[DRF] = self.corner[DLF];
        self.corner[DLF] = tmp;

        self.corner[DLB].flip();
        self.corner[DRB].flip();
        self.corner[DRF].flip();
        self.corner[DLF].flip();

        tmp = self.edge[DB];
        self.edge[DB] = self.edge[DR];
        self.edge[DR] = self.edge[DF];
        self.edge[DF] = self.edge[DL];
        self.edge[DL] = tmp;
    }

    /// Rotate the front face clockwise 90 degrees
    pub fn f(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[ULF];
        self.corner[ULF] = self.corner[DLF];
        self.corner[DLF] = self.corner[DRF];
        self.corner[DRF] = self.corner[URF];
        self.corner[URF] = tmp;

        self.corner[ULF].update(1);
        self.corner[DLF].update(2);
        self.corner[DRF].update(1);
        self.corner[URF].update(2);

        tmp = self.edge[UF];
        self.edge[UF] = self.edge[FL];
        self.edge[FL] = self.edge[DF];
        self.edge[DF] = self.edge[FR];
        self.edge[FR] = tmp;

        self.edge[UF].orientation ^= 1;
        self.edge[FL].orientation ^= 1;
        self.edge[DF].orientation ^= 1;
        self.edge[FR].orientation ^= 1;
    }

    /// Rotate the back face clockwise 90 degrees
    pub fn b(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[ULB];
        self.corner[ULB] = self.corner[URB];
        self.corner[URB] = self.corner[DRB];
        self.corner[DRB] = self.corner[DLB];
        self.corner[DLB] = tmp;

        self.corner[ULB].update(2);
        self.corner[URB].update(1);
        self.corner[DRB].update(2);
        self.corner[DLB].update(1);

        tmp = self.edge[UB];
        self.edge[UB] = self.edge[BR];
        self.edge[BR] = self.edge[DB];
        self.edge[DB] = self.edge[BL];
        self.edge[BL] = tmp;

        self.edge[UB].orientation ^= 1;
        self.edge[BR].orientation ^= 1;
        self.edge[DB].orientation ^= 1;
        self.edge[BL].orientation ^= 1;
    }

    /// Rotate left face clockwise 90 degrees
    pub fn l(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[DLB];
        self.corner[DLB] = self.corner[DLF];
        self.corner[DLF] = self.corner[ULF];
        self.corner[ULF] = self.corner[ULB];
        self.corner[ULB] = tmp;

        self.corner[DLB].update(2);
        self.corner[DLF].update(1);
        self.corner[ULF].update(2);
        self.corner[ULB].update(1);

        tmp = self.edge[BL];
        self.edge[BL] = self.edge[DL];
        self.edge[DL] = self.edge[FL];
        self.edge[FL] = self.edge[UL];
        self.edge[UL] = tmp;
    }

    /// Rotate the right face clockwise 90 degrees
    pub fn r(self: *Cube) void {
        var tmp: Cubie = undefined;

        tmp = self.corner[DRB];
        self.corner[DRB] = self.corner[URB];
        self.corner[URB] = self.corner[URF];
        self.corner[URF] = self.corner[DRF];
        self.corner[DRF] = tmp;

        self.corner[DRB].update(1);
        self.corner[URB].update(2);
        self.corner[URF].update(1);
        self.corner[DRF].update(2);

        tmp = self.edge[BR];
        self.edge[BR] = self.edge[UR];
        self.edge[UR] = self.edge[FR];
        self.edge[FR] = self.edge[DR];
        self.edge[DR] = tmp;
    }

    /// Console print
    pub fn render(self: Cube) void {
        const C = [_]u8{ 'R', 'G', 'B', 'W', 'O', 'Y' };
        var faces: [54]u8 = undefined;

        // edges
        for (0..12) |i| {
            const colors = self.edgeColors(i);
            faces[i * 2 + 0] = C[colors[0]];
            faces[i * 2 + 1] = C[colors[1]];
        }

        // corners
        for (0..8) |i| {
            const colors = self.cornerColors(i);
            faces[24 + i * 3 + 0] = C[colors[0]];
            faces[24 + i * 3 + 1] = C[colors[1]];
            faces[24 + i * 3 + 2] = C[colors[2]];
        }

        // centers
        for (0..6) |i| {
            faces[48 + i] = C[i];
        }

        // 2d mapping of the cube, 54 faces, see map.py
        const map = [54]usize{ 24, 0, 27, 6, 53, 2, 33, 4, 30, 25, 7, 34, 35, 5, 32, 31, 3, 28, 29, 1, 26, 13, 49, 11, 10, 52, 8, 9, 50, 15, 14, 48, 12, 40, 19, 37, 38, 17, 47, 46, 23, 43, 44, 21, 41, 36, 16, 45, 18, 51, 22, 39, 20, 42 };

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

    fn edgeColors(self: Cube, edge: usize) [2]usize {
        const cubie: Cubie = self.edge[edge];

        var colors = switch (cubie.index) {
            UB => [2]usize{ YELLOW, RED },
            UR => [2]usize{ YELLOW, BLUE },
            UF => [2]usize{ YELLOW, ORANGE },
            UL => [2]usize{ YELLOW, GREEN },
            FR => [2]usize{ ORANGE, BLUE },
            FL => [2]usize{ ORANGE, GREEN },
            BL => [2]usize{ RED, GREEN },
            BR => [2]usize{ RED, BLUE },
            DF => [2]usize{ WHITE, ORANGE },
            DL => [2]usize{ WHITE, GREEN },
            DB => [2]usize{ WHITE, RED },
            DR => [2]usize{ WHITE, BLUE },
            else => [2]usize{ WHITE, WHITE },
        };

        if (cubie.orientation == 1) {
            swap(usize, &colors[0], &colors[1]);
        }

        return colors;
    }

    fn cornerColors(self: Cube, corner: usize) [3]usize {
        const cubie: Cubie = self.corner[corner];

        var o0: usize = CO[cubie.index & 1][cubie.orientation][cubie.flipped][0];
        var o1: usize = CO[cubie.index & 1][cubie.orientation][cubie.flipped][1];
        var o2: usize = CO[cubie.index & 1][cubie.orientation][cubie.flipped][2];

        const color = switch (cubie.index) {
            ULB => [3]usize{ YELLOW, GREEN, RED },
            URB => [3]usize{ YELLOW, BLUE, RED },
            URF => [3]usize{ YELLOW, BLUE, ORANGE },
            ULF => [3]usize{ YELLOW, GREEN, ORANGE },
            DLF => [3]usize{ WHITE, GREEN, ORANGE },
            DLB => [3]usize{ WHITE, GREEN, RED },
            DRB => [3]usize{ WHITE, BLUE, RED },
            DRF => [3]usize{ WHITE, BLUE, ORANGE },
            else => [3]usize{ WHITE, WHITE, WHITE },
        };

        return [3]usize{ color[o0], color[o1], color[o2] };
    }
};

pub fn apply(moves: []const u8) void {
    var cube = Cube.init();

    for (moves) |m| {
        switch (m) {
            'F' => cube.f(),
            'B' => cube.b(),
            'R' => cube.r(),
            'L' => cube.l(),
            'U' => cube.u(),
            'D' => cube.d(),
            else => return,
        }
    }

    print("{s}\n", .{moves});
    cube.render();
}

pub fn main() !void {
    // Allocate memory for arguments
    const allocator = std.heap.page_allocator;
    const args = try std.process.argsAlloc(allocator);

    // Parse arguments
    for (args, 0..) |arg, i| {
        if (i > 0) {
            apply(arg);
        }
    }

    // Free the allocated memory for arguments
    std.process.argsFree(allocator, args);
}

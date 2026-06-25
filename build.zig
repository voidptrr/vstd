const std = @import("std");

const c_flags = &.{
    "-std=c17",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
};

fn addCommonSettings(b: *std.Build, module: *std.Build.Module) void {
    module.addIncludePath(b.path("include"));
    module.addIncludePath(b.path("src"));
}

fn pathLessThan(_: void, lhs: []const u8, rhs: []const u8) bool {
    return std.mem.lessThan(u8, lhs, rhs);
}

fn discoverCFiles(b: *std.Build, root_path: []const u8) []const []const u8 {
    const allocator = b.allocator;
    const io = b.graph.io;
    var root = std.Io.Dir.cwd().openDir(io, root_path, .{ .iterate = true }) catch |err| {
        std.debug.panic("unable to open {s}: {s}", .{ root_path, @errorName(err) });
    };
    defer root.close(io);

    var walker = root.walk(allocator) catch @panic("OOM");
    defer walker.deinit();

    var files = std.array_list.Managed([]const u8).init(allocator);
    while (walker.next(io) catch |err| {
        std.debug.panic("unable to walk {s}: {s}", .{ root_path, @errorName(err) });
    }) |entry| {
        if (entry.kind != .file or !std.mem.endsWith(u8, entry.path, ".c")) {
            continue;
        }

        const path = std.fs.path.join(allocator, &.{ root_path, entry.path }) catch @panic("OOM");
        files.append(path) catch @panic("OOM");
    }

    std.sort.heap([]const u8, files.items, {}, pathLessThan);
    return files.toOwnedSlice() catch @panic("OOM");
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const sources = discoverCFiles(b, "src");

    const lib_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    addCommonSettings(b, lib_mod);
    lib_mod.addCSourceFiles(.{
        .files = sources,
        .flags = c_flags,
    });

    const lib = b.addLibrary(.{
        .name = "vstd",
        .root_module = lib_mod,
        .linkage = .static,
    });
    b.installArtifact(lib);
    b.getInstallStep().dependOn(&b.addInstallDirectory(.{
        .source_dir = b.path("include"),
        .install_dir = .header,
        .install_subdir = "",
    }).step);

    const test_step = b.step("test", "Build and run the C test suite with CTest");
    const ctests = b.addSystemCommand(&.{
        "sh",
        "scripts/ctests",
        ".zig-cache/ctests",
    });
    test_step.dependOn(&ctests.step);
}

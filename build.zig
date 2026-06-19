const std = @import("std");

const c_flags = &.{
    "-std=c17",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
};

const sources = &.{
    "src/assert.c",
    "src/datastruct/binary_heap.c",
    "src/datastruct/deque.c",
    "src/datastruct/doubly_linked_list.c",
    "src/datastruct/hash_common.c",
    "src/datastruct/hashmap.c",
    "src/datastruct/hashset.c",
    "src/datastruct/linked_list.c",
    "src/datastruct/string.c",
    "src/datastruct/vector.c",
    "src/memory/allocators/allocator.c",
    "src/memory/allocators/arena.c",
    "src/memory/allocators/general_heap.c",
    "src/memory/allocators/test_allocator.c",
    "src/memory/utils.c",
    "src/testing.c",
};

const tests = &.{
    .{ .name = "datastruct-binary-heap", .path = "tests/datastruct/binary_heap_test.c" },
    .{ .name = "datastruct-deque", .path = "tests/datastruct/deque_test.c" },
    .{ .name = "datastruct-hashmap", .path = "tests/datastruct/hashmap_test.c" },
    .{ .name = "datastruct-hashset", .path = "tests/datastruct/hashset_test.c" },
    .{ .name = "datastruct-linked-list", .path = "tests/datastruct/linked_list_test.c" },
    .{ .name = "datastruct-string", .path = "tests/datastruct/string_test.c" },
    .{ .name = "datastruct-vector", .path = "tests/datastruct/vector_test.c" },
    .{ .name = "memory-arena-alloc", .path = "tests/memory/arena/alloc_test.c" },
    .{ .name = "memory-arena-init", .path = "tests/memory/arena/init_test.c" },
    .{ .name = "memory-arena-realloc", .path = "tests/memory/arena/realloc_test.c" },
    .{ .name = "memory-arena-reset", .path = "tests/memory/arena/reset_test.c" },
    .{ .name = "memory-heap-alloc-free", .path = "tests/memory/heap/alloc_free_test.c" },
    .{ .name = "memory-heap-init", .path = "tests/memory/heap/init_test.c" },
    .{ .name = "memory-heap-realloc", .path = "tests/memory/heap/realloc_test.c" },
};

fn addCommonSettings(b: *std.Build, module: *std.Build.Module) void {
    module.addIncludePath(b.path("include"));
    module.addIncludePath(b.path("src"));
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

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

    const test_step = b.step("test", "Build and run the C test suite");
    inline for (tests) |test_case| {
        const test_mod = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        });
        addCommonSettings(b, test_mod);
        test_mod.addCSourceFile(.{
            .file = b.path(test_case.path),
            .flags = c_flags,
        });

        const exe = b.addExecutable(.{
            .name = test_case.name,
            .root_module = test_mod,
        });
        test_mod.linkLibrary(lib);

        const run = b.addRunArtifact(exe);
        test_step.dependOn(&run.step);
    }
}

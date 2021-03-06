#
# target: libcutf8.so
#

libcutf8_symfile = join_paths(meson.current_source_dir(), 'libcutf8.sym')

libcutf8_deps = [
        dep_cstdaux,
]

libcutf8_private = static_library(
        'cutf8-private',
        [
                'c-utf8.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcutf8_deps,
        pic: true,
)

libcutf8_shared = shared_library(
        'cutf8',
        objects: libcutf8_private.extract_all_objects(),
        dependencies: libcutf8_deps,
        install: not meson.is_subproject(),
        soversion: 0,
        link_depends: libcutf8_symfile,
        link_args: [
                '-Wl,--no-undefined',
                '-Wl,--version-script=@0@'.format(libcutf8_symfile),
        ],
)

libcutf8_dep = declare_dependency(
        include_directories: include_directories('.'),
        link_with: libcutf8_private,
        dependencies: libcutf8_deps,
        version: meson.project_version(),
)

if not meson.is_subproject()
        install_headers('c-utf8.h')

        mod_pkgconfig.generate(
                libraries: libcutf8_shared,
                version: meson.project_version(),
                name: 'libcutf8',
                filebase: 'libcutf8',
                description: project_description,
        )
endif

#
# target: test-*
#

test_api = executable('test-api', ['test-api.c'], link_with: libcutf8_shared)
test('API Symbol Visibility', test_api)

test_basic = executable('test-basic', ['test-basic.c'], dependencies: libcutf8_dep)
test('Basic API Behavior', test_basic)

#
# target: bench-*
#

bench_ascii = executable('bench-ascii', ['bench-ascii.c', 'trivial-utf8.c'], dependencies: libcutf8_dep)
benchmark('ASCII validation', bench_ascii)

bench_multibyte = executable('bench-multibyte', ['bench-multibyte.c', 'trivial-utf8.c'], dependencies: libcutf8_dep)
benchmark('Multi-byte validation', bench_multibyte)

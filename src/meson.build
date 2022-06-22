#
# target: libcutf8.so
#

libcutf8_symfile = join_paths(meson.current_source_dir(), 'libcutf8.sym')

libcutf8_deps = [
        dep_cstdaux,
]

libcutf8_both = both_libraries(
        'cutf8-'+major,
        [
                'c-utf8.c',
        ],
        c_args: [
                '-fvisibility=hidden',
                '-fno-common',
        ],
        dependencies: libcutf8_deps,
        install: not meson.is_subproject(),
        link_args: dep_cstdaux.get_variable('version-scripts') == 'yes' ? [
                '-Wl,--version-script=@0@'.format(libcutf8_symfile),
        ] : [],
        link_depends: libcutf8_symfile,
        soversion: 0,
)

libcutf8_dep = declare_dependency(
        dependencies: libcutf8_deps,
        include_directories: include_directories('.'),
        link_with: libcutf8_both.get_static_lib(),
        version: meson.project_version(),
)

if not meson.is_subproject()
        install_headers('c-utf8.h')

        mod_pkgconfig.generate(
                description: project_description,
                filebase: 'libcutf8-'+major,
                libraries: libcutf8_both.get_shared_lib(),
                name: 'libcutf8',
                version: meson.project_version(),
        )
endif

#
# target: test-*
#

test_api = executable('test-api', ['test-api.c'], link_with: libcutf8_both.get_shared_lib())
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

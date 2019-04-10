project(
        'c-utf8',
        'c',
        version: '1',
        license: 'Apache',
        default_options: [
                'c_std=c11'
        ],
)
project_description = 'UTF-8 Handling in Standard ISO-C11'

add_project_arguments('-D_GNU_SOURCE', language: 'c')
mod_pkgconfig = import('pkgconfig')

sub_cstdaux = subproject('c-stdaux')

dep_cstdaux = sub_cstdaux.get_variable('libcstdaux_dep')

subdir('src')

project(
    'capnp-test', 'c',
    meson_version: '>=0.53.0',
    default_options: [
        'buildtype=debug',
        'warning_level=3', 'werror=false',
        'c_std=c11',
    ]
)

c_args = [
    '-pedantic',
    # Extra warnings.
    '-Werror-implicit-function-declaration',
    '-Wdeprecated',
    '-Wformat-security',
    '-Wmissing-prototypes',
    '-Wpointer-arith',
    '-Wreturn-type',
    '-Wshadow',
    '-Wswitch-enum',
    '-Wundef',
    '-Wunreachable-code',
]

cc = meson.get_compiler('c')
dep_capnp_c = cc.find_library('capnp_c')

# Cap'n Proto command.
capnp_command = find_program('capnp')

capnp_generated_source = custom_target(
    'test.capnp.[ch]',
    output : ['test.capnp.c', 'test.capnp.h'],
    input : 'test.capnp',
    command : [
        capnp_command, 'compile',
        '-oc:@OUTDIR@', '--src-prefix=@CURRENT_SOURCE_DIR@',
        '@INPUT@'
    ]
)

executable(
    'capnp-test',
    sources : ['main.c', capnp_generated_source],
    include_directories : '.',
    dependencies : [dep_capnp_c],
    c_args : c_args,
)

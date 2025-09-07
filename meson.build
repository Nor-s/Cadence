project('thorvg_sandbox', 'cpp',
    default_options:['cpp_std=c++20']
)

platform = get_option('platform')

src_dir = '/'.join(meson.current_source_dir().split('\\'))

if platform == 'web'
    add_project_arguments('-DRESOURCE_DIR=""', language : 'cpp')
    tvg_config_path = 'build/wasm/subprojects/thorvg'
    wasm_link_args = [
      '-sWASM=1',
      '-sUSE_SDL=2',
      '-sUSE_WEBGL2=1',
      '-sFORCE_FILESYSTEM=1',
      '-sALLOW_MEMORY_GROWTH=1',
      '-sFULL_ES3=1',
      '-sMAX_WEBGL_VERSION=2',
      '-sMIN_WEBGL_VERSION=2',
      '--preload-file', '@0@/resources@/'.format(src_dir),
    ]
    # tvg_proj = subproject('thorvg', default_options:['engines=sw,gl', 'log=true', 'loaders=all','savers=all', 'extra=opengl_es,lottie_expressions','partial=false','static=true','threads=false','b_lto=true'])
    tvg_proj = subproject('thorvg', default_options:['engines=sw,gl', 'log=false', 'loaders=all','savers=all', 'extra=opengl_es,lottie_expressions','static=true'])
else
    add_project_arguments('-DRESOURCE_DIR="@0@/resources"'.format(src_dir),
                      language : 'cpp')
    tvg_config_path = 'build/native/subprojects/thorvg'
    wasm_link_args = []
    if host_machine.system() == 'windows'
        add_project_arguments('-DNOMINMAX', language: 'cpp')
        if meson.get_compiler('cpp').get_id() == 'msvc'
            add_project_arguments('/utf-8', language : 'cpp')
        endif
        tvg_proj = subproject('thorvg', default_options:['engines=sw,gl', 'log=false', 'loaders=all','savers=all', 'extra=' ])
    else
        tvg_proj = subproject('thorvg', default_options:['engines=sw,gl', 'log=false', 'loaders=all','savers=all', 'extra=lottie_expressions' ])
    endif
endif


# For Dependency
tvg_lib = tvg_proj.get_variable('thorvg_lib')
tvg_lib_dep = tvg_proj.get_variable('thorvg_lib_dep')
tvg_headers = tvg_proj.get_variable('headers')
tvg_compiler_flags = tvg_proj.get_variable('compiler_flags')
tvg_override_options = tvg_proj.get_variable('override_options')

cmake = import('cmake')
sdl2_dep = dependency('sdl2')
imgui_proj = subproject('imgui')
imgui_dep = imgui_proj.get_variable('imgui_dep')
entt_proj = cmake.subproject('entt')
entt_dep = entt_proj.dependency('EnTT')

spdlog_dep = cmake.subproject('spdlog').dependency('spdlog')

# spdlog_dep = dependency('spdlog', required: true)


example_dep = []
core_dep = []
editor_dep = []
lottie_dep = []
subdir('src/core')
subdir('src/editor')
subdir('src/lottie')
subdir('src/editor/examples')

dep_list =[
    sdl2_dep,
    spdlog_dep,
    imgui_dep,
    entt_dep,
    tvg_lib_dep,
    example_dep, 
    core_dep,
    editor_dep,
    lottie_dep
]

# Src Files
tvg_sandbox_src =[
    'src/main.cpp',
]

tvg_sandbox_inc = include_directories('src', tvg_config_path)

# Define Lib
executable('thorvg_sandbox', 
    tvg_sandbox_src,
    dependencies: dep_list,
    link_args: wasm_link_args,
    include_directories : [tvg_headers, tvg_sandbox_inc],

    cpp_args               : tvg_compiler_flags,
    gnu_symbol_visibility  : 'hidden',
    override_options       : tvg_override_options,
)


# subdir('test')
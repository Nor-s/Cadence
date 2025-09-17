project('Imgui', 'cpp', version: '1.0', default_options: ['cpp_std=c++17'])

# Define include directories
imgui_inc_dirs = include_directories([
  '.',  # Equivalent to ${CMAKE_CURRENT_SOURCE_DIR}
  'icons'
])

# Define source files
imgui_sources = [
  'imconfig.h',
  'GraphEditor.cpp',
  'GraphEditor.h',
  'ImCurveEdit.cpp',
  'ImCurveEdit.h',
  'ImGradient.cpp',
  'ImGradient.h',
  'imgui_demo.cpp',
  'imgui_draw.cpp',
  'imgui_tables.cpp',
  'imgui_widgets.cpp',
  'imgui.cpp',
  'ImGuizmo.cpp',
  'ImGuizmo.h',
  'ImSequencer.cpp',
  'imgui_impl_opengl3.cpp',
  'imgui_impl_sdl2.cpp',
  'ImGuiNotify.hpp',
  'imgInspect.h',
  'imgui_neo_internal.cpp',
  'imgui_neo_internal.h',
  'imgui_neo_sequencer.h',
  'imgui_neo_sequencer.cpp',
  'imgui_helper.cpp',
  'imgui_helper.h',
]

imgui_icons = [
  'icons/icons.cpp',
  'icons/folder.cpp',
  'icons/fontaudio.cpp',
  'icons/fa_brands.cpp',
  'icons/fa_regular.cpp',
  'icons/fa_solid.cpp',
  'icons/webfont.cpp',
  'icons/fork_webfont.cpp',
  'icons/material_icons.cpp',
  'icons/kenney.cpp',
  'icons/StepMath.cpp'
]

# Find SDL2 dependency
sdl2_dep = dependency('sdl2', required: true)

# Build static library
imgui_lib = static_library('imgui',
  sources: imgui_sources + imgui_icons,
  include_directories: imgui_inc_dirs,
  dependencies: [sdl2_dep],
  install: true
)

# Declare the library as a dependency for other targets
imgui_dep = declare_dependency(
  include_directories: imgui_inc_dirs,
  link_with: imgui_lib
)
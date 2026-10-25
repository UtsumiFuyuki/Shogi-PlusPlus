project('Shogi++', 'cpp',
    version: '1.0.0',
    default_options: ['warning_level=3', 'cpp_std=c++23'])

sources = files(
    'src/main.cpp',
    'src/shogi.cpp',
    'src/kif.cpp',
    'src/ui.cpp'
)

incs = 'src/inc'

ftxui_component_dep = dependency('ftxui-component',
  fallback: ['ftxui', 'ftxui_component_dep'],
)

ftxui_incs = 'subprojects/ftxui/include'

executable('shogi++', sources, include_directories : [incs, ftxui_incs], dependencies: [ftxui_component_dep])
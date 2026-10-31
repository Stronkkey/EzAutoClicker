project(
  'EzAutoClicker',
  ['c', 'cpp'],
  meson_version : '>= 1.1',
  version : '1.0',
  default_options : [
    'cpp_std=c++23',
    'warning_level=3',
    ],
)


sources = files(
  'src/app.cpp',
  'src/input/clicker_factory.cpp',
  'src/input/evdev/hotkey_listener_evdev.cpp',
  'src/input/evdev/clicker_uinput.cpp',
  'src/input/hotkey_listener_factory.cpp',
  'src/input/threaded_clicker.cpp',
  'src/input/x11/hotkey_listener_x11.cpp',
  'src/input/x11/clicker_x11.cpp',
  'src/input/threaded_clicker.cpp',
  'src/input/wlr/clicker_wlr.cpp',
  'src/main.cpp',
  'src/window.cpp'
)

build_args = [
  '-Wall',
  '-Wextra',
  '-pedantic',
  '-Werror',
]


features = []
x11_feature = '-D EZAUTOCLICKER_X11_INPUT_ENABLED=1'
wlr_feature = '-D EZAUTOCLICKER_ENABLE_WLR=1'


libevdev_dep = dependency('libevdev')
gtkmm4_dep = dependency('gtkmm-4.0')

dependencies = [
  libevdev_dep,
  gtkmm4_dep,
]

gnome = import('gnome')
resources = gnome.compile_resources(
  'EzAutoClicker-Resources',
  'data/EzAutoClicker.gresource.xml',
  source_dir : 'data',
  c_name : 'EzAutoClicker',
)


if get_option('x11').enabled()
  x11_dep = dependency('x11', required : true)
  xtst_dep = dependency('xtst', required : true)

  dependencies += x11_dep
  dependencies += xtst_dep
  features += x11_feature
endif

if get_option('wlr').enabled()
  wayland_client_dep = dependency('wayland-client', required : true)
  wayland_protocols_dep = dependency('wayland-protocols', required: true)
  wlr_protocols_dep = dependency('wlr-protocols', required : true)
  wayland_scanner_prog = find_program('wayland-scanner', required : true)

  dependencies += wayland_client_dep
  dependencies += wayland_protocols_dep
  dependencies += wlr_protocols_dep
  features += wlr_feature

  wlr_dir = wlr_protocols_dep.get_variable('pkgdatadir')

  protocol_xml = [
    wlr_dir / 'unstable/wlr-virtual-pointer-unstable-v1.xml',
  ]

  client_header = generator(wayland_scanner_prog,
    output: '@BASENAME@-client-protocol.h',
    arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
  )

  private_code = generator(wayland_scanner_prog,
    output: '@BASENAME@-protocol.c',
    arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
  )

  foreach xml : protocol_xml
    sources += client_header.process(xml)
    sources += private_code.process(xml)
  endforeach
endif


sources += resources
cpp_args = build_args + features


ez_auto_clicker_exe = executable(
  'EzAutoClicker',
  sources,
  dependencies : dependencies,
  install : false,
  cpp_args : cpp_args,
)

test('basic', ez_auto_clicker_exe, timeout: -1)
test('TestX11', ez_auto_clicker_exe, timeout: -1, args: ['--clicker=x11', '--hotkey-listener=x11'])
test('Wlr', ez_auto_clicker_exe, timeout : -1, args : ['--clicker=wlr'])
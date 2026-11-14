project(
  'EzAutoClicker',
  ['c', 'cpp'],
  meson_version : '>= 1.1',
  version : '1.0',
  default_options : [
    'cpp_std=c++20',
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
  'src/main.cpp',
  'src/process.cpp',
  'src/window.cpp'
)

extra_sources = files()

x11_sources = files(
  'src/input/x11/clicker_x11.cpp',
  'src/input/x11/hotkey_listener_x11.cpp',
)

wlr_sources = files(
  'src/input/wlr/clicker_wlr.cpp',
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
run_as_root_feature = '-D EZAUTOCLICKER_ALLOW_RUNNING_AS_ROOT=1'


libevdev_dep = dependency('libevdev')
gtkmm4_dep = dependency('gtkmm-4.0')

dependencies = [
  libevdev_dep,
  gtkmm4_dep,
]

extra_dependencies = []

x11_dependencies = [
  dependency('x11', required : false),
  dependency('xtst', required : false),
]

wlr_dependencies = [
  dependency('wayland-client', required : false),
]

gnome = import('gnome')
resources = gnome.compile_resources(
  'EzAutoClicker-Resources',
  'data/EzAutoClicker.gresource.xml',
  source_dir : 'data',
  c_name : 'EzAutoClicker',
)


if get_option('x11').enabled()
  dependency('x11', required : true)
  dependency('xtst', required : true)

  extra_dependencies += x11_dependencies
  extra_sources += x11_sources
  features += x11_feature
endif

if get_option('wlr').enabled()
  dependency('wayland-client', required : true)
  wayland_scanner_prog = find_program('wayland-scanner', required : true)

  features += wlr_feature
  protocol_xml = files(
    'protocols/unstable/wlr-virtual-pointer-unstable-v1.xml',
  )

  client_header = generator(wayland_scanner_prog,
    output: '@BASENAME@-client-protocol.h',
    arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
  )

  private_code = generator(wayland_scanner_prog,
    output: '@BASENAME@-protocol.c',
    arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
  )

  foreach xml : protocol_xml
    wlr_sources += client_header.process(xml)
    wlr_sources += private_code.process(xml)
  endforeach
  extra_dependencies += wlr_dependencies
  extra_sources += wlr_sources
endif

if get_option('run_as_root').allowed()
  features += run_as_root_feature
endif


sources += resources

main_exe = executable(
  'EzAutoClicker',
  sources + extra_sources,
  dependencies : dependencies + extra_dependencies,
  install : false,
  cpp_args : build_args + features,
)

x11_exe = executable(
  'TestX11',
  sources + x11_sources,
  dependencies : dependencies + x11_dependencies,
  install : false,
  cpp_args : build_args + x11_feature
)

wlr_exe = executable(
  'TestWlr',
  sources + wlr_sources,
  dependencies : dependencies + wlr_dependencies,
  install : false,
  cpp_args : build_args + wlr_feature,
)

test('basic', main_exe, timeout: -1)
test('TestX11', x11_exe, timeout: -1, args: ['--clicker=x11', '--hotkey-listener=x11'])
test('TestWlr', wlr_exe, timeout : -1, args : ['--clicker=wlr'])

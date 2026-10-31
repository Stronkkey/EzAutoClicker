project(
  'EzAutoClicker',
  'cpp',
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
  'src/main.cpp',
  'src/window.cpp',
)

build_args = [
  '-Wall',
  '-Wextra',
  '-pedantic',
  '-Werror',
]


features = []
x11_feature = '-D EZAUTOCLICKER_X11_INPUT_ENABLED=1'


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
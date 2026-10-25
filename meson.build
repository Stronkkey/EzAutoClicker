project(
  'EzAutoClicker',
  'cpp',
  meson_version : '>= 1.3.0',
  version : '1.0',
  default_options : ['warning_level=3', 'cpp_std=c++23'],
)

sources = [
  'src/main.cpp',
  'src/window.cpp',
  'src/app.cpp',
  'src/input/input_player_x11.cpp',
  'src/input/input_player_uinput.cpp',
  'src/input/threaded_input.cpp',
  'src/input/hotkey_listener_evdev.cpp',
  'src/input/hotkey_listener_x11.cpp'
]

build_args = [
  '-Wall',
  '-Wextra',
  '-pedantic',
]

features = []
x11_feature = '-D EZAUTOCLICKER_X11_INPUT_ENABLED=1'

dependencies = [
  dependency('gtkmm-4.0'),
  dependency('libevdev'),
]

if get_option('x11_input').enabled()
  dependencies += dependency('x11')
  dependencies += dependency('xtst')
  features += x11_feature
endif

gnome = import('gnome')
resources = gnome.compile_resources(
	'EzAutoClicker-Resources',
	'data/EzAutoClicker.gresource.xml',
	source_dir: 'data',
	c_name: 'EzAutoClicker',
)

sources += resources

exe = executable(
  'EzAutoClicker',
  sources,
  dependencies : dependencies,
  install : false,
  cpp_args: build_args + features
)


test_x11= executable(
  'TestX11',
  sources,
  dependencies : dependencies,
  install : false,
  cpp_args: build_args + x11_feature
)

test('basic', exe, timeout: -1)
test('TestX11', test_x11, timeout: -1)
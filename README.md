# EzAutoClicker

EzAutoClicker - automate mouse clicks at fixed intervals.
Works on both X11 and Wayland. Inspired by [OP Auto Clicker](https://www.opautoclicker.com/).

## Quick Start

Install the required dependencies first.
See [Installing dependencies on common distros](#installing-dependencies-on-common-distros).

```sh
git clone https://github.com/Stronkkey/EzAutoClicker.git
cd EzAutoClicker
meson setup build
meson compile -C build
sudo meson install -C build # Optional: Installs system-wide.
```

## Building

### Dependencies

The following are required to build the project.

- A C++20 compiler
- pkgconf
- Meson >= 1.1
- GTKmm >= 4.0
- libevdev

Additional dependencies are also required when building with additional backends.

| Backend | Library | Requirement |
|---------|---------|-------------|
| X11 | X11 XTest | X server must support the XTest extension |
| Wlr | [wlr-protocols](https://gitlab.freedesktop.org/wlroots/wlr-protocols) | Compositor must support the [wlr-virtual-pointer-unstable-v1](https://wayland.app/protocols/wlr-virtual-pointer-unstable-v1) protocol. |

#### Installing dependencies on common distros

##### Debian / Ubuntu

```sh
sudo apt install -y --no-install-recommends \
  build-essential \
  g++ \
  libevdev-dev \
  libgtkmm-4.0-dev \
  libwayland-dev \
  libwayland-bin \
  libxtst-dev \
  meson \
  pkg-config
```

##### Arch Linux

```sh
sudo pacman -S --needed --noconfirm \
  base-devel \
  gtkmm-4.0 \
  libevdev \
  meson \
  pkgconf \
  libxtst \
  wayland
```

##### Fedora

```sh
sudo dnf install \
  @c-development \
  gtkmm4.0-devel \
  meson \
  libevdev-devel \
  libXtst \
  wayland-devel
```

### Compiling

Setup the build directory and run `meson compile`.

```sh
meson setup build
meson compile -C build
```

Building with [docker](https://www.docker.com/) is also possible.
Run `build_docker.sh` to build with docker.

```sh
./build_docker.sh
```

You can also install EzAutoClicker system-wide.

```sh
sudo meson install -C build # Usually installs to /usr
```

## Usage

```text
$ EzAutoClicker --help
Usage:
  EzAutoClicker [OPTION…]

EzAutoClicker - automate mouse clicks at fixed intervals.

Help Options:
  -h, --help                           Show help options
  --help-all                           Show all help options
  --help-gapplication                  Show GApplication options

Application Options:
  -t, --toggle                         Start or stop the autoclicker in a running instance.
  --milliseconds=MILLISECONDS          Click interval in milliseconds.
  --seconds=SECONDS                    Click interval in seconds.
  --minutes=MINUTES                    Click interval in minutes.
  --hours=HOURS                        Click interval in hours.
  --mouse-button=left|middle|right     Mouse button which is clicked.
  --click-type=single|double           Single (default) sends only one click. Double sends two clicks at the same times.
  --pick-location=X,Y                  Send clicks at given screen position. Overrides the use-current-location option.
  --repeat-until-stopped               Sets Clicker to only stop when manually stopped. Overrides repeat-count argument.
  --repeat-count=REPEAT_COUNT          Amount of times to repeat before automatically stopping clicker.
  --clicker=uinput|wlr|x11|auto        The clicker backend used to send clicks.
  --hotkey-listener=x11|evdev|auto     The hotkey listener backend used to listen for the hotkey.

Press the configured hotkey (default F5) to start or stop clicking.
```

## License

GPL v3. See [LICENSE](LICENSE).

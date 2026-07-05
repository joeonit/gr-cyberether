# gr-cyberether

A GNU Radio OOT module to bring [CyberEther](https://github.com/luigifcruz/CyberEther) GPU-accelerated visualization sinks.

This is a **GSoC 2026** project, currently in active
development.


## Status

A Fully working lineplot and waterfall sinks that supports mutable inputs, Multi-sink in one window (shared and flexible mosaic grid) and more!

Coming soon: constellation and spectrogram + a distribution channel for more seamless install

## Requirements

- GNU Radio (mian or 3.10)
- CyberEther ≥ 1.4.0 (has to be built from source for now)
- C++20
- Platforms: macOS, linux , windows(still untested)


## Build from source

### 1. Build CyberEther into a prefix

```bash
git clone https://github.com/luigifcruz/CyberEther
cd CyberEther

meson setup build \
  --prefix="$HOME/.local/cyberether-1.4.0" \
  --buildtype release

meson install -C build
```

That gives you `$HOME/.local/cyberether-1.4.0/lib/pkgconfig/jetstream.pc`
plus the headers and shared library.

> If you're tracking CyberEther main instead of a tag, expect API breakage
> until it stabilises. Pin to a tag (`git checkout v1.4.0`) for a known-good build.

### 2. Build gr-cyberether

You'll need to point pkg-config at the CyberEther prefix and tell CMake
to install into the same place GR lives.

```bash
git clone https://github.com/joeonit/gr-cyberether
cd gr-cyberether
mkdir build && cd build

PKG_CONFIG_PATH=$HOME/.local/cyberether-1.4.0/lib/pkgconfig:$PKG_CONFIG_PATH \
cmake .. \
  -DCMAKE_INSTALL_PREFIX=$GR_PREFIX \
  -DCMAKE_BUILD_TYPE=Release

cmake --build . --parallel
cmake --install .
```

Replace `$GR_PREFIX` with the prefix where your GNU Radio is installed


### 3. Verify the install

```bash
python -c "from gnuradio import cyberether; print(cyberether.cyber_lineplot_sink_c)"
```


## Mentors

- [Luigi Cruz](https://github.com/luigifcruz) — CyberEther
- [Håkon Vågsether](https://github.com/haakov) — GNU Radio


## License

GPL-3.0-or-later, matching GNU Radio.

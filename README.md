# gr-cyberether

A GNU Radio OOT module to bring [CyberEther](https://github.com/luigifcruz/CyberEther) GPU-accelerated visualization sinks.

This is a **GSoC 2026** project, currently in active
development.


## Status

A Fully working lineplot and waterfall sinks that supports mutable inputs, Multi-sink in one window (shared and flexible mosaic grid) and more!

Coming soon: constellation and spectrogram + a distribution channel for more seamless install

## Requirements

- GNU Radio (mian or 3.10)
- CyberEther ≥ 1.7.0 (has to be built from source for now), tested against 1.9.1
- Meson ≥ 1.11 to build CyberEther (Homebrew may still ship 1.10 — `brew upgrade meson`)
- C++20
- Platforms: macOS, linux , windows(still untested)


## Install from a release

Prebuilt tarballs for Linux x86_64 and macOS arm64 are attached to each
[release](https://github.com/joeonit/gr-cyberether/releases). They bundle
`libjetstream`, so they do **not** need a separate CyberEther install.

```bash
tar -xzf gr-cyberether-linux-x86_64.tar.gz
cd gr-cyberether-linux-x86_64
./install.sh
```

`install.sh` finds your GNU Radio prefix, checks that the bundled Python
binding matches the Python your GNU Radio runs, installs, and verifies.
`--dry-run`, `--prefix`, and `--uninstall` are supported.

> **Python versions matter.** The tarball ships a compiled CPython extension,
> which loads only on the version it was built for — 3.12 for the Linux
> artifact (Ubuntu 24.04 apt GNU Radio), 3.14 for macOS (Homebrew GNU Radio).
> If your GNU Radio uses a different Python (radioconda ships 3.12 on macOS),
> `install.sh` will say so and you should build from source instead.


## Build from source

### 1. Build CyberEther into a prefix

```bash
git clone https://github.com/luigifcruz/CyberEther
cd CyberEther
git checkout v1.9.1

meson setup build \
  --prefix="$HOME/.local/cyberether-1.9.1" \
  --buildtype release

meson install -C build
```

That gives you `$HOME/.local/cyberether-1.9.1/lib/pkgconfig/jetstream.pc`
plus the headers and shared library.

> If you're tracking CyberEther main instead of a tag, expect API breakage
> until it stabilises. Pin to a tag (`git checkout v1.9.1`) for a known-good build.

### 2. Build gr-cyberether

You'll need to point pkg-config at the CyberEther prefix and tell CMake
to install into the same place GR lives.

```bash
git clone https://github.com/joeonit/gr-cyberether
cd gr-cyberether
mkdir build && cd build

PKG_CONFIG_PATH=$HOME/.local/cyberether-1.9.1/lib/pkgconfig:$PKG_CONFIG_PATH \
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

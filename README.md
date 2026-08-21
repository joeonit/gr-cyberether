# gr-cyberether

A GNU Radio OOT module to bring [CyberEther](https://github.com/luigifcruz/CyberEther) GPU-accelerated visualization sinks.
Built during **Google Summer of Code 2026** with GNU Radio.


https://github.com/user-attachments/assets/5b59230c-6c3f-447a-a3ee-516d6cf90a9c



https://github.com/user-attachments/assets/2e4cec69-c0a4-444d-8c8e-c43507c1132e

---

## Blocks

| Block                            | Input            | What it draws                                              |
| -------------------------------- | ---------------- | ---------------------------------------------------------- |
| `cyber_lineplot_sink`            | complex or float | line plot, time or frequency domain                        |
| `cyber_waterfall_sink`           | complex or float | scrolling waterfall                                        |
| `cyber_constellation_sink`       | complex          | IQ scatter plot                                            |
| `cyber_spectrum_analyzer_sink` † | complex          | spectrum line above a waterfall, sharing one FFT           |
| `cyber_range`                    | —                | a slider in the control panel, driving a GRC variable live |

> † **Awaiting upstream merge.** The spectrum analyzer sink, and drawing
> `cyber_range` sliders both depend on CyberEther changes that are written and under review and probably would be available with CyberEther V 2.0.0

---

## Requirements

- GNU Radio 3.10 or main
- CyberEther >= 1.7.0 (tested against 1.9.1)
- Meson >= 1.11 to build CyberEther
- C++20
- Linux, macOS and Windows.

---

## Install

### From source (recommended)

The Python bindings are compiled against *your* GNU Radio and *your* Python.

**1. Build CyberEther into a prefix**

```bash
git clone https://github.com/luigifcruz/CyberEther
cd CyberEther
git checkout v1.9.1

meson setup build --prefix="$HOME/.local/cyberether-1.9.1" --buildtype release
meson install -C build
```

**2. Build gr-cyberether against it**

```bash
git clone https://github.com/joeonit/gr-cyberether
cd gr-cyberether

PKG_CONFIG_PATH=$HOME/.local/cyberether-1.9.1/lib/pkgconfig:$PKG_CONFIG_PATH \
cmake -B build -DCMAKE_INSTALL_PREFIX="$(gnuradio-config-info --prefix)"

cmake --build build -j
cmake --install build
```

Windows is more involved, see [windows-build.md](windows-build.md).

**3. Verify**

```bash
python -c "from gnuradio import cyberether; print(cyberether.cyber_lineplot_sink_c)"
```

### From a release tarball

Prebuilt archives for Linux x86_64, macOS arm64 and Windows x86_64 are attached
to each [release](https://github.com/joeonit/gr-cyberether/releases). They
bundle `libjetstream`, so no CyberEther install is needed.

```bash
tar -xzf gr-cyberether-linux-x86_64.tar.gz
cd gr-cyberether-linux-x86_64
./install.sh
```

`install.sh` finds your GNU Radio, checks compatibility, installs and verifies.
It supports `--dry-run`, `--prefix`, `--force` and `--uninstall`.

> **These only work if your GNU Radio uses the same Python they were built
> against**

Radioconda distribution may come soon!

---

## Quick start

1. Set the Options block's **Generate Options** to `cyberether_gui`.
2. Drop a sink, connect it, give it a **GUI Hint** like `0, 0`.
3. Press Run.

No Python Snippet block, no manual window handling, no code outside the
flowgraph.

### Layout

Sinks share one window. Placement uses the same syntax as the qtgui blocks:

```
"row, col"                       one cell
"row, col, row_span, col_span"   a rectangular block of cells
```

```
┌─────────────────────────────────────────────┐
│  noise ▁▂▃▄▅▆▇  offset ▁▂▃▄▅▆▇              │  ← cyber_range controls
├───────────────┬──────────────┬──────────────┤
│               │    "0,1"     │    "0,2"     │
│  "0,0,2,1"    ├──────────────┴──────────────┤
│               │        "1,1,1,2"            │
├───────────────┴─────────────────────────────┤
│               "2,0,1,3"                     │
└─────────────────────────────────────────────┘
```

A sink with no hint is auto-placed; a malformed hint falls back to
auto-placement rather than failing the flowgraph. Controls sit above the grid
and never take a plot slot.

---

## Mentors

- [Luigi Cruz](https://github.com/luigifcruz) — CyberEther
- [Håkon Vågsether](https://github.com/haakov) — GNU Radio

---

## License

GPL-3.0-or-later.

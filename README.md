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

### Windows with radioconda

Open the **Radioconda PowerShell Prompt**, then load Visual Studio's x64 build
environment. GNU Radio and the OOT module must use the same compiler, Python,
and pybind11 ABI.

```powershell
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * `
  -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
  -property installationPath
& "$vs\Common7\Tools\Launch-VsDevShell.ps1" `
  -Arch amd64 -HostArch amd64 -SkipAutomaticLocation

conda install -y -c conda-forge cmake ninja pkg-config gnuradio boost-cpp "pybind11=2.13"
python -m pip install "meson>=1.11" PyYAML numpy mapbox_earcut
```

Install the Vulkan SDK, then build CyberEther 1.9.1 into a separate prefix.
Until CyberEther tags a release that exports its public `Superluminal` API,
the one-line replacement below is required for external Windows consumers.

```powershell
$work = (Resolve-Path .).Path
$cePrefix = "$work\cyberether-prefix"

git clone --depth 1 --branch v1.9.1 https://github.com/luigifcruz/CyberEther.git
$header = "$work\CyberEther\include\jetstream\superluminal.hh"
$source = [IO.File]::ReadAllText($header)
$source = $source.Replace('class Superluminal {', 'class JETSTREAM_API Superluminal {')
[IO.File]::WriteAllText($header, $source, [Text.UTF8Encoding]::new($false))

meson setup CyberEther cyberether-build --vsenv `
  --prefix="$cePrefix" -Dbuildtype=release -Ddefault_library=shared `
  -Db_vscrt=static_from_buildtype -Dpython=false -Dtests=false `
  -Dexamples=false -Dremote=disabled -Dinference=disabled
meson compile -C cyberether-build
meson install -C cyberether-build
```

Build and install gr-cyberether into radioconda. Native Windows `pkg-config`
uses semicolons between paths and works most reliably with forward slashes.

```powershell
git clone https://github.com/joeonit/gr-cyberether.git
Set-Location gr-cyberether

$env:PKG_CONFIG_PATH = ("$cePrefix\lib\pkgconfig").Replace('\', '/')
$env:PATH = "$cePrefix\bin;$env:CONDA_PREFIX\Library\bin;$env:PATH"

# GNU Radio 3.10 checks the stored MD5 for manually maintained bindings.
$pairs = @('cyber_lineplot_sink', 'cyber_waterfall_sink')
foreach ($name in $pairs) {
  $binding = "python\cyberether\bindings\${name}_python.cc"
  $header = "include\gnuradio\cyberether\${name}.h"
  $hash = (Get-FileHash $header -Algorithm MD5).Hash.ToLowerInvariant()
  $text = [IO.File]::ReadAllText($binding)
  $text = [regex]::Replace($text,
    '(BINDTOOL_HEADER_FILE_HASH\()[0-9a-fA-F]+(\))', "`${1}$hash`${2}", 1)
  [IO.File]::WriteAllText($binding, $text, [Text.UTF8Encoding]::new($false))
}

cmake -S . -B build -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_INSTALL_PREFIX="$env:CONDA_PREFIX\Library" `
  -DCMAKE_PREFIX_PATH="$env:CONDA_PREFIX\Library" `
  -DCMAKE_CXX_FLAGS="/Zc:preprocessor /DNOGDI /DBOOST_ALL_NO_LIB" `
  -DGR_PYTHON_DIR="$env:CONDA_PREFIX\Lib\site-packages" `
  -DPython_EXECUTABLE="$env:CONDA_PREFIX\python.exe" `
  -DENABLE_PYTHON=ON -DENABLE_DOXYGEN=OFF
cmake --build build
cmake --install build
Copy-Item "$cePrefix\bin\jetstream.dll" "$env:CONDA_PREFIX\Library\bin" -Force
```

Verify from the same activated environment:

```powershell
python -c "from gnuradio import cyberether; b=cyberether.cyber_lineplot_sink_c(128, 'test'); assert b.to_basic_block(); print('OK')"
```

The Windows release workflow performs the same build and staged-package smoke
test. The temporary source adjustments can be removed after the corresponding
CyberEther export and binding-hash updates are released.

### Linux and macOS

#### 1. Build CyberEther into a prefix

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

#### 2. Build gr-cyberether

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


#### 3. Verify the install

```bash
python -c "from gnuradio import cyberether; print(cyberether.cyber_lineplot_sink_c)"
```


## Mentors

- [Luigi Cruz](https://github.com/luigifcruz) — CyberEther
- [Håkon Vågsether](https://github.com/haakov) — GNU Radio


## License

GPL-3.0-or-later, matching GNU Radio.

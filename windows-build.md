# Building gr-cyberether on Windows

Windows needs more setup than Linux or macOS: GNU Radio, CyberEther and this
module must all agree on the compiler, the Python interpreter and the pybind11
ABI, and two upstream rough edges still need working around by hand.

If you only want the blocks and not a source build, use the Windows archive
attached to a [release](https://github.com/joeonit/gr-cyberether/releases)
instead.

## Prerequisites

- [radioconda](https://github.com/ryanvolz/radioconda) for Windows
- Visual Studio 2022 with the **Desktop development with C++** workload
- The [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)

## Build

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

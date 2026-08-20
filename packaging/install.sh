#!/usr/bin/env bash
#
# Install gr-cyberether from a release tarball into a GNU Radio prefix.

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PREFIX=""
DRY_RUN=0
UNINSTALL=0
FORCE=0

RED=$'\033[31m'; GREEN=$'\033[32m'; YELLOW=$'\033[33m'; BOLD=$'\033[1m'; OFF=$'\033[0m'
if [[ ! -t 1 ]]; then RED=""; GREEN=""; YELLOW=""; BOLD=""; OFF=""; fi

info()  { printf '%s\n' "$*"; }
ok()    { printf '%s✓%s %s\n' "$GREEN" "$OFF" "$*"; }
warn()  { printf '%s!%s %s\n' "$YELLOW" "$OFF" "$*"; }
die()   { printf '%s✗ %s%s\n' "$RED" "$*" "$OFF" >&2; exit 1; }

usage() {
    cat <<'EOF'
Install gr-cyberether from a release tarball into a GNU Radio prefix.

  ./install.sh                     install into the detected GNU Radio prefix
  ./install.sh --prefix /some/dir  install somewhere else
  ./install.sh --dry-run           show what would happen, change nothing
  ./install.sh --uninstall         remove a previous install
  ./install.sh --force             install despite a Python version mismatch
EOF
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --prefix)    PREFIX="${2:-}"; shift 2 ;;
        --prefix=*)  PREFIX="${1#*=}"; shift ;;
        --dry-run)   DRY_RUN=1; shift ;;
        --uninstall) UNINSTALL=1; shift ;;
        --force)     FORCE=1; shift ;;
        -h|--help)   usage ;;
        *)           die "unknown option: $1 (try --help)" ;;
    esac
done

[[ -d "$HERE/lib" && -d "$HERE/share/gnuradio" ]] || \
    die "run this from inside the extracted tarball (no lib/ or share/gnuradio/ next to this script)"

BUNDLED_PY_DIR=$(find "$HERE/lib" -maxdepth 1 -type d -name 'python3.*' | head -1)
[[ -n "$BUNDLED_PY_DIR" ]] || die "no lib/python3.* directory in this tarball"
BUNDLED_PY=$(basename "$BUNDLED_PY_DIR")
BUNDLED_PY_VER=${BUNDLED_PY#python}

PREFIX_EXPLICIT=0
if [[ -z "$PREFIX" ]]; then
    command -v gnuradio-config-info >/dev/null 2>&1 || die \
"GNU Radio not found on PATH (no gnuradio-config-info).

Install GNU Radio first, or pass one explicitly:
    ./install.sh --prefix /path/to/gnuradio/prefix"
    PREFIX=$(gnuradio-config-info --prefix)
    GR_VERSION=$(gnuradio-config-info --version 2>/dev/null || echo "unknown")
    ok "found GNU Radio $GR_VERSION at $PREFIX"
else
    PREFIX_EXPLICIT=1
    ok "using prefix $PREFIX"
fi

PREFIX="${PREFIX%/}"
[[ -d "$PREFIX" ]] || die "prefix does not exist: $PREFIX"

# The interpreter that matters is the one that can import gnuradio, not
# whatever python3 happens to be first on PATH.
GR_PY_VER=""
for cand in "$PREFIX/bin/python3" python3 python; do
    command -v "$cand" >/dev/null 2>&1 || continue
    if "$cand" -c 'import gnuradio' >/dev/null 2>&1; then
        GR_PY_VER=$("$cand" -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')
        GR_PY_BIN=$(command -v "$cand")
        break
    fi
done

if [[ -z "$GR_PY_VER" ]]; then
    warn "could not find a Python that imports gnuradio; skipping the version check"
elif [[ "$GR_PY_VER" != "$BUNDLED_PY_VER" ]]; then
    msg="Python version mismatch.

  this tarball was built for : Python $BUNDLED_PY_VER
  your GNU Radio uses        : Python $GR_PY_VER  ($GR_PY_BIN)

A compiled CPython extension only loads on the version it was built for, so
the GRC blocks would fail to import even though the files installed fine.

Options:
  - use the release artifact built for Python $GR_PY_VER, if one exists
  - install a GNU Radio that uses Python $BUNDLED_PY_VER
  - build gr-cyberether from source against your own GNU Radio
    (see https://github.com/joeonit/gr-cyberether#build-from-source)"
    if [[ $FORCE -eq 1 ]]; then
        warn "$msg"
        warn "--force given, continuing anyway"
    else
        die "$msg

Re-run with --force to install regardless."
    fi
else
    ok "Python $GR_PY_VER matches this build"
fi

# Only follow the interpreter's own site-packages when the prefix was detected
# too; an explicit --prefix must not write into some other GNU Radio.
TARGET_SITE=""
if [[ $PREFIX_EXPLICIT -eq 0 && -n "$GR_PY_VER" ]] && command -v "${GR_PY_BIN:-}" >/dev/null 2>&1; then
    TARGET_SITE=$("$GR_PY_BIN" -c 'import gnuradio, os; print(os.path.dirname(os.path.dirname(gnuradio.__file__)))' 2>/dev/null || true)
fi
[[ -n "$TARGET_SITE" ]] || TARGET_SITE="$PREFIX/lib/$BUNDLED_PY/site-packages"

MANIFEST="$PREFIX/share/gnuradio/gr-cyberether-install-manifest.txt"

if [[ $UNINSTALL -eq 1 ]]; then
    [[ -f "$MANIFEST" ]] || die "no install manifest at $MANIFEST — nothing to uninstall"
    info "${BOLD}Removing files listed in $MANIFEST${OFF}"
    while IFS= read -r f; do
        [[ -e "$f" ]] || continue
        if [[ $DRY_RUN -eq 1 ]]; then info "  would remove $f"; else rm -f "$f"; fi
    done < "$MANIFEST"
    # Generated at import time, so never in the manifest.
    while IFS= read -r -d '' d; do
        if [[ $DRY_RUN -eq 1 ]]; then info "  would remove $d"; else rm -rf "$d"; fi
    done < <(find "$PREFIX" -type d -path '*/gnuradio/cyberether/__pycache__' -print0 2>/dev/null)

    if [[ $DRY_RUN -eq 0 ]]; then
        rm -f "$MANIFEST"
        ok "uninstalled"
    else
        info "(dry run, nothing removed)"
    fi
    exit 0
fi

info ""
info "${BOLD}Installing gr-cyberether${OFF}"
info "  prefix        : $PREFIX"
info "  python module : $TARGET_SITE"
info ""

if [[ $DRY_RUN -eq 1 ]]; then
    info "${BOLD}Dry run — nothing will be written.${OFF}"
else
    mkdir -p "$(dirname "$MANIFEST")"
    : > "$MANIFEST.tmp"
fi

copy_tree() {
    local src="$1" dst="$2"
    [[ -d "$src" ]] || return 0
    local rel
    while IFS= read -r -d '' f; do
        rel="${f#"$src"/}"
        if [[ $DRY_RUN -eq 1 ]]; then
            info "  would install $dst/$rel"
        else
            mkdir -p "$dst/$(dirname "$rel")"
            cp -p "$f" "$dst/$rel"
            printf '%s\n' "$dst/$rel" >> "$MANIFEST.tmp"
        fi
    done < <(find "$src" -type f -print0)
}

copy_tree "$HERE/lib"     "$PREFIX/lib"
copy_tree "$HERE/include" "$PREFIX/include"
copy_tree "$HERE/share"   "$PREFIX/share"

BUNDLED_SITE="$HERE/lib/$BUNDLED_PY/site-packages"
if [[ -d "$BUNDLED_SITE" && "$TARGET_SITE" != "$PREFIX/lib/$BUNDLED_PY/site-packages" ]]; then
    copy_tree "$BUNDLED_SITE" "$TARGET_SITE"
fi

if [[ $DRY_RUN -eq 1 ]]; then
    info ""
    info "Dry run complete."
    exit 0
fi

mv "$MANIFEST.tmp" "$MANIFEST"
ok "files installed ($(wc -l < "$MANIFEST" | tr -d ' ') entries recorded in the manifest)"

info ""
info "${BOLD}Verifying${OFF}"

VERIFY_PY="${GR_PY_BIN:-python3}"

# __init__.py swallows ImportError to stay importable without the extension, so
# importing the package proves nothing — check for a symbol the binding defines.
VERIFY_SRC='
import sys
from gnuradio import cyberether
if getattr(cyberether, "_cyberether_python", None) is None:
    sys.exit("extension-missing")
if not hasattr(cyberether, "cyber_lineplot_sink_c"):
    sys.exit("symbols-missing")
print(cyberether.__file__)
'
if VERIFY_OUT=$(PYTHONPATH="$TARGET_SITE:${PYTHONPATH:-}" "$VERIFY_PY" -c "$VERIFY_SRC" 2>&1); then
    ok "gnuradio.cyberether loaded with its compiled extension"
else
    warn "gnuradio.cyberether did not load correctly: ${VERIFY_OUT##*$'\n'}"
    warn "the files installed, but the Python binding is not usable."
    warn "usually this means the extension was built for a different Python"
    warn "than the one GNU Radio runs, or $TARGET_SITE is not on PYTHONPATH:"
    warn "    export PYTHONPATH=\"$TARGET_SITE:\$PYTHONPATH\""
fi

BLOCK_DIR="$PREFIX/share/gnuradio/grc/blocks"
COUNT=$(find "$BLOCK_DIR" -name 'cyberether_*.yml' 2>/dev/null | wc -l | tr -d ' ')
if [[ "$COUNT" -gt 0 ]]; then
    ok "$COUNT GRC block definitions installed"
else
    warn "no cyberether block definitions found under $BLOCK_DIR"
fi

info ""
info "${BOLD}Done.${OFF} Try it:"
info "    gnuradio-companion $PREFIX/share/gnuradio/examples/cyberether/lineplot_demo.grc"
info ""
info "In GRC set the Options block's 'Generate Options' to 'cyberether_gui'."
info "To remove: ./install.sh --uninstall"

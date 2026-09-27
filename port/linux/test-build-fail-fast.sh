#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
TMP_ROOT=$(mktemp -d)
trap 'rm -rf "$TMP_ROOT"' EXIT HUP INT TERM

make_stub() {
    name=$1
    body=$2
    printf '#!/bin/sh\n%s\n' "$body" > "$TMP_ROOT/bin/$name"
    chmod +x "$TMP_ROOT/bin/$name"
}

mkdir -p "$TMP_ROOT/bin"

init_root="$TMP_ROOT/init"
mkdir -p "$init_root/config"
cp "$SCRIPT_DIR/init.sh" "$init_root/init.sh"
printf '#define PIKA_CONFIG_ENABLE 1\n' > "$init_root/config/pika_config_default.h"
printf '#!/bin/sh\nexit 23\n' > "$init_root/update-compiler.sh"
chmod +x "$init_root/update-compiler.sh"
git -C "$init_root" init -q

set +e
(cd "$init_root" && PATH="$TMP_ROOT/bin:$PATH" sh init.sh)
status=$?
set -e
if [ "$status" -ne 23 ]; then
    echo "init.sh did not preserve generator failure: $status" >&2
    exit 1
fi

make_root="$TMP_ROOT/make"
mkdir -p "$make_root/build/test" "$make_root/package/pikascript"
cp "$SCRIPT_DIR/only_make.sh" "$make_root/only_make.sh"
make_stub ninja 'exit 29'

set +e
(cd "$make_root" && PATH="$TMP_ROOT/bin:$PATH" sh only_make.sh)
status=$?
set -e
if [ "$status" -ne 29 ]; then
    echo "only_make.sh did not preserve Ninja failure: $status" >&2
    exit 1
fi
if [ -e "$make_root/package/pikascript/pika" ]; then
    echo "only_make.sh continued after Ninja failure" >&2
    exit 1
fi

echo "Linux build scripts fail fast: OK"

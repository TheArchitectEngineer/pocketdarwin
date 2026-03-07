#!/usr/bin/env sh
# shellcheck shell=sh

if [ -n "${BASH_SOURCE:-}" ]; then
  _mk_script="${BASH_SOURCE%/*}"
else
  _mk_script="$(dirname "$0")"
fi

MAKEKIT_ROOT="${MAKEKIT_ROOT:-$(cd "$_mk_script" && pwd)}"
export MAKEKIT_ROOT
export MAKEKIT_MAKEFILES="${MAKEKIT_ROOT}/makefiles"

export CC="${CC:-clang}"
export CXX="${CXX:-clang++}"
export AR="${AR:-llvm-ar}"
export RANLIB="${RANLIB:-llvm-ranlib}"

unset _mk_script

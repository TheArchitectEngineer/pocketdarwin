set -l _mk_dir (dirname (status --current-filename))
if not set -q MAKEKIT_ROOT
    set -gx MAKEKIT_ROOT (cd $_mk_dir; pwd)
end
set -gx MAKEKIT_MAKEFILES "$MAKEKIT_ROOT/makefiles"

if not set -q CC
    set -gx CC clang
end
if not set -q CXX
    set -gx CXX clang++
end
if not set -q AR
    set -gx AR llvm-ar
end
if not set -q RANLIB
    set -gx RANLIB llvm-ranlib
end

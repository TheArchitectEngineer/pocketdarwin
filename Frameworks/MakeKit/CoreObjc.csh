set _mk_dir = `dirname "$0"`
if ( $?MAKEKIT_ROOT == 0 ) then
    setenv MAKEKIT_ROOT `cd "$_mk_dir" && pwd`
endif
setenv MAKEKIT_MAKEFILES "${MAKEKIT_ROOT}/makefiles"

if ( $?CC == 0 ) then
    setenv CC clang
endif
if ( $?CXX == 0 ) then
    setenv CXX clang++
endif
if ( $?AR == 0 ) then
    setenv AR llvm-ar
endif
if ( $?RANLIB == 0 ) then
    setenv RANLIB llvm-ranlib
endif

unset _mk_dir

# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_compile_stdcxx.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_COMPILE_STDCXX(VERSION, [ext|noext], [mandatory|optional])
#
# DESCRIPTION
#
#   Check for baseline language coverage in the compiler for the given
#   C++ standard.  If the current language level is less than that
#   requested, an error is produced if MANDATORY is set, otherwise a
#   warning is produced.
#
#   The VERSION argument may be either "11", "14", "17", or "20".  The
#   optional second argument may be either "ext" or "noext".  The
#   optional third argument controls whether the check is mandatory or
#   optional.
#
#   This macro is a simplified version of Autoconf's AC_CXX_COMPILE_STDCXX_11,
#   AC_CXX_COMPILE_STDCXX_14, and AC_CXX_COMPILE_STDCXX_17 macros.
#
# LICENSE
#
#   Copyright (c) 2015 Moritz Klammler <moritz.klammler@yahoo.de>
#   Copyright (c) 2016 Krzysztof Kozlowski <krzk@kernel.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 7

m4_define([_AX_CXX_COMPILE_STDCXX_testbody_new_in_11], [[
  namespace test_std_11
  {
    template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    struct check2
    {
      check2() { }
      virtual ~check2() { }
      virtual void f() { }
    };

    using check_pair = std::pair<int, int>;
    static_assert(std::is_same_v<decltype(std::make_pair(1, 2)), check_pair>, "pair type mismatch");

    enum class foo { a, b, c };
    static_assert(std::is_same_v<std::underlying_type_t<foo>, int>, "underlying type mismatch");

    void test_lambda()
    {
      auto lambda = [](int x) { return x + 1; };
      static_assert(lambda(2) == 3, "lambda test failed");
    }

    void test_auto()
    {
      auto i = 0;
      static_assert(std::is_same_v<decltype(i), int>, "auto type mismatch");
    }

    void test_range_for()
    {
      int arr[] = {1, 2, 3};
      int sum = 0;
      for (auto x : arr)
        sum += x;
      static_assert(sum == 6, "range for test failed");
    }
  }
]])

m4_define([_AX_CXX_COMPILE_STDCXX_testbody_new_in_14], [[
  namespace test_std_14
  {
    template<class T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    struct check2
    {
      check2() { }
      virtual ~check2() { }
      virtual void f() { }
    };

    using check_pair = std::pair<int, int>;
    static_assert(std::is_same_v<decltype(std::make_pair(1, 2)), check_pair>, "pair type mismatch");

    void test_lambda()
    {
      auto lambda = [](auto x) { return x + 1; };
      static_assert(lambda(2) == 3, "lambda test failed");
    }

    void test_auto()
    {
      auto i = 0;
      static_assert(std::is_same_v<decltype(i), int>, "auto type mismatch");
    }

    void test_range_for()
    {
      int arr[] = {1, 2, 3};
      int sum = 0;
      for (auto x : arr)
        sum += x;
      static_assert(sum == 6, "range for test failed");
    }
  }
]])

m4_define([_AX_CXX_COMPILE_STDCXX_testbody_new_in_17], [[
  namespace test_std_17
  {
    template<class T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    struct check2
    {
      check2() { }
      virtual ~check2() { }
      virtual void f() { }
    };

    using check_pair = std::pair<int, int>;
    static_assert(std::is_same_v<decltype(std::make_pair(1, 2)), check_pair>, "pair type mismatch");

    void test_lambda()
    {
      auto lambda = [](auto x) { return x + 1; };
      static_assert(lambda(2) == 3, "lambda test failed");
    }

    void test_auto()
    {
      auto i = 0;
      static_assert(std::is_same_v<decltype(i), int>, "auto type mismatch");
    }

    void test_range_for()
    {
      int arr[] = {1, 2, 3};
      int sum = 0;
      for (auto x : arr)
        sum += x;
      static_assert(sum == 6, "range for test failed");
    }

    void test_structured_bindings()
    {
      std::pair<int, int> p{1, 2};
      auto [a, b] = p;
      static_assert(a == 1 && b == 2, "structured bindings test failed");
    }
  }
]])

m4_define([_AX_CXX_COMPILE_STDCXX_testbody_new_in_20], [[
  namespace test_std_20
  {
    template<class T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    struct check2
    {
      check2() { }
      virtual ~check2() { }
      virtual void f() { }
    };

    using check_pair = std::pair<int, int>;
    static_assert(std::is_same_v<decltype(std::make_pair(1, 2)), check_pair>, "pair type mismatch");

    void test_lambda()
    {
      auto lambda = [](auto x) { return x + 1; };
      static_assert(lambda(2) == 3, "lambda test failed");
    }

    void test_auto()
    {
      auto i = 0;
      static_assert(std::is_same_v<decltype(i), int>, "auto type mismatch");
    }

    void test_range_for()
    {
      int arr[] = {1, 2, 3};
      int sum = 0;
      for (auto x : arr)
        sum += x;
      static_assert(sum == 6, "range for test failed");
    }

    void test_structured_bindings()
    {
      std::pair<int, int> p{1, 2};
      auto [a, b] = p;
      static_assert(a == 1 && b == 2, "structured bindings test failed");
    }
  }
]])

AC_DEFUN([AX_CXX_COMPILE_STDCXX], [dnl
  m4_if([$1], [11], [ax_cxx_compile_alternatives="11"], 
        [$1], [14], [ax_cxx_compile_alternatives="14"], 
        [$1], [17], [ax_cxx_compile_alternatives="17"], 
        [$1], [20], [ax_cxx_compile_alternatives="20"], 
        [m4_fatal([invalid standard specified])])
  m4_if([$2], [], [ax_cxx_compile_cxx$1_required=true],
        [$2], [mandatory], [ax_cxx_compile_cxx$1_required=true],
        [$2], [optional], [ax_cxx_compile_cxx$1_required=false],
        [m4_fatal([invalid second argument passed to AX_CXX_COMPILE_STDCXX])])
  AC_LANG_PUSH([C++])
  m4_if([$1], [11], [ax_cxx_compile_cxx$1_testbody="_AX_CXX_COMPILE_STDCXX_testbody_new_in_11"],
        [$1], [14], [ax_cxx_compile_cxx$1_testbody="_AX_CXX_COMPILE_STDCXX_testbody_new_in_14"],
        [$1], [17], [ax_cxx_compile_cxx$1_testbody="_AX_CXX_COMPILE_STDCXX_testbody_new_in_17"],
        [$1], [20], [ax_cxx_compile_cxx$1_testbody="_AX_CXX_COMPILE_STDCXX_testbody_new_in_20"])
  AC_CACHE_CHECK([whether $CXX supports C++$1 with $ax_cxx_compile_alternatives], [ax_cv_cxx_compile_cxx$1],
    [ac_save_CXX="$CXX"
     CXX="$CXX $CXXFLAGS $CPPFLAGS"
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([_AX_CXX_COMPILE_STDCXX_testbody_new_in_$1])],
       [ax_cv_cxx_compile_cxx$1=yes],
       [ax_cv_cxx_compile_cxx$1=no])
     CXX="$ac_save_CXX"])
  AC_LANG_POP([C++])
  m4_if(ax_cxx_compile_cxx$1_required, true, [
    AS_IF([test "$ax_cv_cxx_compile_cxx$1" != yes], [
      AC_MSG_ERROR([*** A compiler with support for C++$1 language features is required.])
    ])
  ])
])

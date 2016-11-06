set r=g:\spsl\googletest-release-1.8.0\googletest
set v=Release
cmake -DGTEST_ROOT=%r% -DGTEST_LIBRARY=%r%\msvc\gtest\%v%\gtest.lib -DGTEST_MAIN_LIBRARY=%r%\msvc\gtest\%v%\gtest_main.lib ..
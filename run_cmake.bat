set GTEST=g:\spsl\googletest-release-1.8.0
set buildcfg=Release
cmake -DGTEST_LIBRARY=%GTEST%\build\%buildcfg%\gtest.lib -DGTEST_MAIN_LIBRARY=%GTEST%\build\%buildcfg%\gtest_main.lib -DGTEST_INCLUDE_DIR=%GTEST%\googletest\include ..

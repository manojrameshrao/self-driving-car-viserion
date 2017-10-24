echo off
set working_dir=%cd%

:::::::: Check if the our working directory contains spaces
set working_dir_no_spaces=%working_dir%
set working_dir_no_spaces=%working_dir_no_spaces: =%

:::::::: If we've got a space, then quit here.
if not "%working_dir_no_spaces%"=="%working_dir%" (
    echo ------------------------------------------------------------
    echo Your directory path contains a space.
    echo Try putting this package without a space in the path
    echo ------------------------------------------------------------
    echo Recommended location is your C drive itself
    echo Common mistake is when people put this at their desktop
    echo and unfortunately their name has a space :(
    echo ------------------------------------------------------------
    echo Pressing any key will exit this process.
    echo ------------------------------------------------------------
    pause
    exit
)

:::::::: JAVA
set PATH=%working_dir%\toolchain\jre\bin;%PATH%;

:::::::: Toolchain
set PATH=%working_dir%\toolchain\bin;%PATH%
set PATH=%working_dir%\binutils;%PATH%

:::::::: Doxygen & Graphviz
set PATH=%working_dir%\doxygen\doxygen\bin;%PATH%
set PATH=%working_dir%\doxygen\graphviz\bin;%PATH%

:::::::: Python
set PATH=%working_dir%\python-3.5.0-embed-win32;%PATH%

:::::::: echo %PATH%
start eclipse32\eclipse.exe -clean -data projects
:: start cd .
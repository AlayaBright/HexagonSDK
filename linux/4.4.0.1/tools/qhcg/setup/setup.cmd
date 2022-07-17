rem This script checks if virtual environment exist
rem   and set corresponding Python interpreter to PY variable.
rem If virtual environment doesn't exist create it
rem   and install required modules.

rem Python3 is needed, supported versions are 3.4, 3.5, 3.6 and 3.7

set "REQ_MOD="
set PYTHON_APP=python3.exe
call :check_python_version
if not defined REQ_MOD (
    set PYTHON_APP=python.exe
    call :check_python_version
)
if not defined REQ_MOD (
    echo [ERROR]: In order to use QHL HVX code generator Python 3.7 and pip3 are required.
    echo          Please install them first and add to your PATH variable.
    exit /B
)

rem Check for Python virtual environment

rem Virtual environment directory named 'venv' under current (setup) directory
set VIRT_ENV_DIR=venv

rem Virtual environment full path
set SCRIPT_PATH=%~dp0
set VIRT_ENV_PATH=%SCRIPT_PATH%%VIRT_ENV_DIR%

rem Use python and pip3 from virtual environment
set PIP=%VIRT_ENV_PATH%\Scripts\pip3.exe

rem Test if virtual environment exist
rem TODO: Find better way for checking for virtual environment
if not exist %PIP% (
    echo.
    echo The virtual environment doesn't exist
    echo Create a brand new virtual environment in %VIRT_ENV_PATH%
    echo.

    where /q pip3.exe
    if errorlevel 1 (
        echo [ERROR]: In order to use QHL HVX code generator pip3 is required.
        echo          Please install pip3 first and add to your PATH variable.
        exit /B
    )

    python -m venv %VIRT_ENV_PATH%

    if not errorlevel 0 (
        echo [ERROR]: Problem with creating virtual environment
        exit /b
    )

    echo Upgrade PIP
    %PIP% install --upgrade pip > NUL 2> NUL
    %VIRT_ENV_PATH%\Scripts\python.exe -m pip install --upgrade pip > NUL 2> NUL

    echo Install required modules from %REQ_MOD% ...
    %PIP% install -r %SCRIPT_PATH%\%REQ_MOD% > NUL

    if errorlevel 0 (
        echo All modules installed successfully
    ) else (
        echo [ERROR]: Installation of required modules failed
        exit /b
    )

    echo Done.
)

set PY=%VIRT_ENV_PATH%\Scripts\python.exe

exit /b
rem ---------------

:check_python_version

rem Check for supported Python version

%PYTHON_APP% -c "import sys; s = 103 if sys.version_info.major is not 3 else 133 if sys.version_info.minor < 4 else 138 if sys.version_info.minor > 7 else 100; sys.exit(s)" 2> NUL
set PYTHON_CHECK_STATUS=%errorlevel%
rem 100 : ok python 3.4-3.7
rem 103 : not supported python like 2.x
rem 133 : not supported python 3.3 or earlier
rem 138 : not supported python 3.8 or later
rem else - no python exe at all

if %PYTHON_CHECK_STATUS% equ 100 (
    set REQ_MOD=req3.txt
) else (
    if %PYTHON_CHECK_STATUS% equ 138 goto prn_error_msg
    if %PYTHON_CHECK_STATUS% equ 133 goto prn_error_msg
)

exit /B
rem ---------------

:prn_error_msg

echo [ERROR]: Not all Python modules supported by provided version:
%PYTHON_APP% --version
echo.

exit /B
rem ---------------
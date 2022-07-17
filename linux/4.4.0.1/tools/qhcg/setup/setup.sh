#! /bin/bash

# This script checks if virtual environment exist
#   and set corresponding Python interpreter to PY variable.
# If virtual environment doesn't exist create it
#   and install required modules.

# Python3 is needed, supported versions are 3.4, 3.5, 3.6 and 3.7

# Check for python3, Exit if missing
if [ ! -x "$(command -v python3)" ] ; then
    echo "[ERROR]: In order to use QHL HVX code generator Python3 and pip3 are required."
    echo "         Please install them first and add to your PATH variable."
    echo "         Supported Python versions: 3.4, 3.5, 3.6, and 3.7"
    exit 1
fi

# Check for supported Python version

PYTHON_VERSION=`python3 -c 'import sys; minor = sys.version_info.minor; s = 103 if sys.version_info.major is not 3 else 133 if minor < 4 else 134 if minor is 4 else 138 if minor > 7 else 100; print(s)'`
# 100 : ok python 3.4-3.7
# 103 : not supported python like 2.x
# 133 : not supported python 3.3 or earlier
# 138 : not supported python 3.8 or later
# else - no python exe at all

unset REQ_MOD

if [ $PYTHON_VERSION == 100 -o $PYTHON_VERSION == 134 ] ; then
    REQ_MOD=req3.txt
elif [ $PYTHON_VERSION == 138 -o $PYTHON_VERSION == 133 ] ; then
    echo "[ERROR]: Not all Python modules supported by provided version:" `python3 --version`
fi

if [ -z "$REQ_MOD" ] ; then
    echo "[ERROR]: In order to use QHL HVX code generator Python3 and pip3 are required."
    echo "         Please install them first and add to your PATH variable."
    echo "         Supported Python versions: 3.4, 3.5, 3.6, and 3.7"
    exit 1
fi

# Check for Python virtual environment

# Virtual environment directory named 'venv' under current (setup) directory
VIRT_ENV_DIR=venv

# Virtual environment full path
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
VIRT_ENV_PATH=$SCRIPT_PATH/$VIRT_ENV_DIR

# Use python and pip3 from virtual environment
PIP=$VIRT_ENV_PATH/bin/pip3

# Test if virtual environment exist
# TODO: Find better way for checking for virtual environment
if [ ! -f $PIP ] ; then
    echo ""
    echo "The virtual environment doesn't exist."
    echo "Create a brand new virtual environment in $VIRT_ENV_PATH"
    echo ""

    if [ ! -x "$(command -v pip3)" ] ; then
        echo "[ERROR]: In order to use QHL HVX code generator pip3 is required."
        echo "         Please install pip3 first (e.g. sudo apt install python3-pip)"
        exit 1
    fi

    # patch for Tkinter (Python 3.4.3 modules)
    python3 -c 'import tkinter' 2>/dev/null
    if [ ! $? == 0 ] ; then
        echo "[ERROR]: In order to run QHCG tool with modules from Python 3.4 the Tkinter is required."
        echo "         Please install Tkinter module first (e.g. sudo apt install python3-tk)"
        exit 1
    fi

    python3 -m venv $VIRT_ENV_PATH
    if [ ! $? == 0 ] ; then
        echo "[ERROR]: Problem with creating virtual environment"
        # if [ $PYTHON_VERSION == 134 ] ; then
            echo "Note: In order to create virtual environment with Python 3.4"
            echo "      please install venv module using the following command."
            echo ""
            echo "      sudo apt-get install python3.4-venv"
            echo ""
        # fi
        exit 1
    fi

    echo "Try to upgrade PIP"
    $PIP install --upgrade pip==19.3

    echo "Install required modules from $REQ_MOD ..."
    $PIP install -r $SCRIPT_PATH/$REQ_MOD

    if [ $? == 0 ] ; then
        echo "All modules installed successfully"
    else
        echo "[ERROR]: Installation of required modules failed"
        exit 1
    fi

    echo Done.
fi

PY=$VIRT_ENV_PATH/bin/python3

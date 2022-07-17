#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

if [[ ${machine} = "Linux" ]];
then
    STR=${PWD}
else
    STR=$("${HEXAGON_SDK_ROOT}\\tools\\utils\\gow-0.8.0\\bin\\pwd.EXE")
fi

SUB=' '

if [[ "$STR" == *"$SUB"* ]]; then
  echo "Error! Current working directory path contains spaces. GNU make does not build the directory if there is a space in its directory path."
  exit
fi

if [ ${machine} = "Linux" ]
then
    CMAKE_ROOT=${CMAKE_ROOT_PATH}
    CMAKE=${CMAKE_ROOT}/bin/cmake
    ANDROID_NDK_ROOT=${ANDROID_ROOT_DIR}
    HEXAGON_CMAKE_MAKE_PROGRAM=/usr/bin/make
    HEXAGON_CMAKE_MAKE_FILETYPE="-GUnix Makefiles"
    HEXAGON_CMAKE_ROOT=${HEXAGON_SDK_ROOT}/build/cmake
    _src=$(dirname "$(realpath $0)")
    # Default Definition
    DSP_VARIANT=${DEFAULT_DSP_ARCH}
    BUILD_TYPE=${DEFAULT_BUILD}
    HLOS_VARIANT=${DEFAULT_HLOS_ARCH}
    QNX_TOOLS_VERSION="toolv710"
    TOOLS_VERSION=${DEFAULT_TOOLS_VARIANT}
    NO_QURT=${DEFAULT_NO_QURT_INC}
    BUILD_TARGET_NAME=""
    BUILD_TAG=""
    DOMAIN=3
    TREE_CLEAN=0
    CLEAN_ONLY=0
    release="$(cat /etc/os-release | grep -i version_id )"
    version=$(tr -dc '0-9.' <<< $release)
    verbose=0
    android_api_level=26
else

    CMAKE_ROOT="$CMAKE_ROOT_PATH"
    CMAKE="${CMAKE_ROOT}\\bin\\cmake.exe"
    ANDROID_NDK_ROOT=${ANDROID_ROOT_DIR}
    HEXAGON_CMAKE_MAKE_PROGRAM="${HEXAGON_SDK_ROOT}\\tools\\utils\\gow-0.8.0\\bin\\make.exe"
    HEXAGON_CMAKE_MAKE_FILETYPE="-GMinGW Makefiles"
    HEXAGON_CMAKE_ROOT=${HEXAGON_SDK_ROOT}/build/cmake
    _src=$(dirname $0)
    # Default Definition
    DSP_VARIANT=${DEFAULT_DSP_ARCH}
    BUILD_TYPE=${DEFAULT_BUILD}
    QNX_TOOLS_VERSION="toolv710"
    HLOS_VARIANT=${DEFAULT_HLOS_ARCH}
    TOOLS_VERSION=${DEFAULT_TOOLS_VARIANT}
    NO_QURT=${DEFAULT_NO_QURT_INC}
    BUILD_TARGET_NAME=""
    DOMAIN=3
    BUILD_TAG=""
    TREE_CLEAN=0
    CLEAN_ONLY=0
    version=""
    verbose=0
    android_api_level=26
fi


doClean() {
    # Clean the Current Project Dir
    rm -rf ${_src}/${BUILD_TAG}
    # clean the Dependency Libs
    # Currently can clean the libs shipped in the SDK
    # if any custom libs added, paths are to be added below to clean
    for lib in "$@"
    do
        rm -rf ${HEXAGON_SDK_ROOT}/ipc/asyncq/${lib}/${BUILD_TAG}
        rm -rf ${HEXAGON_SDK_ROOT}/ipc/fastrpc/${lib}/${BUILD_TAG}
        rm -rf ${HEXAGON_SDK_ROOT}/libs/${lib}/${BUILD_TAG}
        rm -rf ${HEXAGON_SDK_ROOT}/utils/${lib}/${BUILD_TAG}
    done
    echo "Cleaning of directories completed"
}


doBuild() {

    unknown_argc=0
    unknown_argv=()
    for i in "$@"
    do
    case $i in
        BUILD=*)
        BUILD_TYPE="${i#*=}"
        shift # past argument=value
        ;;
        DSP_ARCH=*)
        DSP_VARIANT="${i#*=}"
        shift # past argument=value
        ;;
        NO_QURT_INC=*)
        NO_QURT="${i#*=}"
        shift # past argument=value
        ;;
        DOMAIN_FLAG=*)
        DOMAIN="${i#*=}"
        shift
        ;;
        API_LEVEL=*)
        android_api_level="${i#*=}"
        shift
        ;;
        VERBOSE=*)
        verbose="${i#*=}"
        shift
        ;;
        HLOS_ARCH=*)
        HLOS_VARIANT="${i#*=}"
        shift # past argument=value
        ;;
        OPENWRT=*)
        OPENWRT_VARIANT="${i#*=}"
        shift
        ;;
        SOFT_FLOAT=*)
        SOFT_FLOAT_VARIANT="${i#*=}"
        shift
        ;;
        QNX_TOOLS_VARIANT=*)
        QNX_TOOLS_VERSION="${i#*=}"
        shift
        ;;
        V=*)
        BUILD_TAG="${i#*=}"
        shift # past argument=value
        ;;
        TEST_NAME=*)
        TARGET_NAME="${i#*=}"
        shift # past argument=value
        ;;
        tree=*)
        TREE_CLEAN=${i#*=}
        shift # past argument with no value
        ;;
        *)
        ((++unknown_argc))
        unknown_argv+=("$i")
        BUILD_TAG_NAME="$i" # custom option
        shift
        ;;
    esac
    done

    if [[ "$unknown_argc" -gt 1 ]];
    then
        echo "Invalid arguments: Multiple Platforms not allowed in single build -- ${unknown_argv[@]}"
        return 1
    fi

    echo "BUILD_TYPE          = ${BUILD_TYPE}"
    echo "DSP_VARIANT         = ${DSP_VARIANT}"
    echo "NO_QURT             = ${NO_QURT}"
    echo "HLOS_VARIANT        = ${HLOS_VARIANT}"
    echo "DOMAIN              = ${DOMAIN}"
    echo "BUILD_TAG           = ${BUILD_TAG}"
    echo "BUILD_TAG_NAME      = ${BUILD_TAG_NAME}"
    ver1=$(tr -dc '0-9.' <<< $HEXAGON_TOOLS_ROOT)
    #echo $ver1
    if [[ "${ver1}" == *"8.3"* ]]; then
        echo "Using Hexagon tools version 8.3.x!"
        TOOLS_VERSION="toolv83"
    elif [[ "${ver1}" == *"8.2"* ]]; then
        echo "Using Hexagon tools version 8.2.x!"
        TOOLS_VERSION="toolv82"
    elif [[ "${ver1}" == *"8.1"* ]]; then
        echo "Using Hexagon tools version 8.1.x!"
        TOOLS_VERSION="toolv81"
    fi

    if [ "$BUILD_TAG" != "" ];
    then
        # Build Tag provided use it
        # Replace Release with ReleaseG
        #BUILD_TAG=${BUILD_TAG/_Release/_ReleaseG}
        if [[ $BUILD_TAG == *"android"* ]];
        then
            echo "BUILD_TAG           = ${BUILD_TAG}"
            if [ "$BUILD_TAG_NAME" = "android_clean" ];
            then
                #TODO: clean the Example and the dependencies
                 echo "Cleaning Android project and dependencies"
                 CLEAN_ONLY=1
            else
                # invoke android Build Coammand
                doAndroid $BUILD_TAG
            fi
        elif [[ $BUILD_TAG == *"qnx"* ]];
        then
            echo "BUILD_TAG           = ${BUILD_TAG}"
            if [ "$BUILD_TAG_NAME" = "qnx_clean" ];
            then
                echo "Cleaning QNX project and dependencies"
                CLEAN_ONLY=1
            else
                source ${HEXAGON_SDK_ROOT}/addons/qnx/build/cmake/qnx_configure.bash
                doQnx $BUILD_TAG
            fi
        elif [[ $BUILD_TAG == *"UbuntuARM"* ]]
        then
            echo "BUILD_TAG           = ${BUILD_TAG}"
            if [ "$BUILD_TAG_NAME" = "UbuntuARM_clean" ];
            then
                echo "Cleaning Ubuntu project and dependencies"
                CLEAN_ONLY=1
            else
                doUbuntuARM $BUILD_TAG
            fi
        elif [[ $BUILD_TAG == *"hexagon"* ]];
        then
            # Remove dynamic from build Tag to support backward campatibility
            BUILD_TAG=${BUILD_TAG/_dynamic}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            if [ "$BUILD_TAG_NAME" = "hexagonsim" ];
            then
                doHexagon $BUILD_TAG ${TARGET_NAME}_q
            elif [ "$BUILD_TAG_NAME" = "hexagon" ];
            then
                doHexagon $BUILD_TAG ${TARGET_NAME}_skel
            elif [ "$BUILD_TAG_NAME" = "hexagon_clean" ];
            then
                 # TODO: Clean hexagon Variant
                 echo "Cleaning Hexagon project and dependencies"
                 CLEAN_ONLY=1
            fi
        elif [[ $BUILD_TAG == *"windows"* ]];
        then
            echo "BUILD_TAG           = ${BUILD_TAG}"
            if [ "$BUILD_TAG_NAME" = "windows_clean" ];
            then
                echo "Cleaning Windows project and dependencies"
                CLEAN_ONLY=1
            else
                source ${HEXAGON_SDK_ROOT}/addons/wos/build/cmake/wos_configure.bash
                doWindows $BUILD_TAG
            fi
        fi
    elif [ "$BUILD_TAG" = "" ];
    then
        # Build Tag Not Provided Construct it
        if [ "$BUILD_TAG_NAME" = "android" ];
        then
            # Construct Android Build Tag
            BUILD_TAG=android_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_device
            doAndroid $BUILD_TAG
        elif [ "$BUILD_TAG_NAME" = "android_clean" ];
        then
            BUILD_TAG=android_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            echo "Cleaning Android project and dependencies"
            CLEAN_ONLY=1
        elif [ "$BUILD_TAG_NAME" = "qnx" ];
        then
            # Construct QNX Build Tag
            source ${HEXAGON_SDK_ROOT}/addons/qnx/build/cmake/qnx_configure.bash
            BUILD_TAG=qnx_${BUILD_TYPE}_aarch${HLOS_VARIANT}_${QNX_TOOLS_VERSION}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_device
            doQnx $BUILD_TAG
        elif [ "$BUILD_TAG_NAME" = "qnx_clean" ];
        then
            BUILD_TAG=qnx_${BUILD_TYPE}_aarch${HLOS_VARIANT}_${QNX_TOOLS_VERSION}
            echo "Cleaning QNX project and dependencies"
            CLEAN_ONLY=1
        elif [ "$BUILD_TAG_NAME" = "ubuntuARM" ];
        then
            if [ "$HLOS_VARIANT" = "64" ];
            then
                BUILD_TAG=UbuntuARM_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            else
                BUILD_TAG=UbuntuARM_${BUILD_TYPE}
            fi
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_device
            doUbuntuARM $BUILD_TAG
        elif [ "$BUILD_TAG_NAME" = "ubuntuARM_clean" ];
        then
            if [ "$HLOS_VARIANT" = "64" ];
            then
                BUILD_TAG=UbuntuARM_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            else
                BUILD_TAG=UbuntuARM_${BUILD_TYPE}
            fi
            echo "Cleaning ubuntuARM project and dependencies"
            CLEAN_ONLY=1
        elif [ "$BUILD_TAG_NAME" = "hexagon" ];
        then
            # Construct Hexagon Build Tag
            BUILD_TAG=hexagon_${BUILD_TYPE}_${TOOLS_VERSION}_${DSP_VARIANT}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_skel
            doHexagon $BUILD_TAG ${TARGET_NAME}_skel
        elif [ "$BUILD_TAG_NAME" = "hexagonsim" ];
        then
            # Construct Hexagon Build Tag and run on simulator
            BUILD_TAG=hexagon_${BUILD_TYPE}_${TOOLS_VERSION}_${DSP_VARIANT}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_q
            doHexagon $BUILD_TAG  ${TARGET_NAME}_q
        elif [ "$BUILD_TAG_NAME" = "hexagon_clean" ];
        then
            BUILD_TAG=hexagon_${BUILD_TYPE}_${TOOLS_VERSION}_${DSP_VARIANT}
            echo "Cleaning Hexagon project and dependencies"
            CLEAN_ONLY=1
        elif [ "$BUILD_TAG_NAME" = "windows" ];
        then
            # Construct windows Build Tag
            BUILD_TAG=windows_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_device
            source ${HEXAGON_SDK_ROOT}/addons/wos/build/cmake/wos_configure.bash
            doWindows $BUILD_TAG
        elif [ "$BUILD_TAG_NAME" = "windows_clean" ];
        then
            BUILD_TAG=windows_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            echo "Cleaning Windows project and dependencies"
            CLEAN_ONLY=1
        elif [[ "$BUILD_TAG_NAME" == *"clean"* ]]
        then
            HLOS_NAME=${BUILD_TAG_NAME//_clean}
            if [ "$HLOS_VARIANT" = "64" ]
            then
                BUILD_TAG=${HLOS_NAME}_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            else
                BUILD_TAG=${HLOS_NAME}_${BUILD_TYPE}
            fi
            echo "BUILD_TAG           = ${BUILD_TAG}"
            CLEAN_ONLY=1
        else
            HLOS_NAME=${BUILD_TAG_NAME}

            if [ "$HLOS_VARIANT" = "64" ]
            then
                BUILD_TAG=${HLOS_NAME}_${BUILD_TYPE}_aarch${HLOS_VARIANT}
            else
                BUILD_TAG=${HLOS_NAME}_${BUILD_TYPE}
            fi
            echo "BUILD_TAG           = ${BUILD_TAG}"
            BUILD_TARGET_NAME=${TARGET_NAME}_device
            doCustom ${BUILD_TAG}
        fi
    fi
# end of Function doBuild()
}


doAndroid() {
    # Android Build to run on the Device

    V="$1"
    #_build=${_src}/${V}
    ANDROID_BUILD_FULLPATH=${_src}/${V}
    PREBUILT_LIB_DIR=${V//_Debug}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_ReleaseG}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_Release}
    _build=${_src}/${V}

    if [ "$TREE_CLEAN" = "1" ];
    then
        # Delete Projrct Dir
        rm -rf ${_build}
    fi

    DSP_TYPE=CDSP
    if [ "$DOMAIN" = "0" ];
    then
        DSP_TYPE=ADSP
    elif [ "$DOMAIN" = "1" ];
    then
        DSP_TYPE=MDSP
    fi
    echo "Building for domain $DSP_TYPE"

    MY_BUILD_OPTION=Release
    if [ "$BUILD_TYPE" = "Debug" ];
    then
        MY_BUILD_OPTION=Debug
    fi

    cd ${_src} && \
    ${CMAKE} \
    -H${_src} \
    -B${_build} \
    -DANDROID_NDK=${ANDROID_NDK_ROOT} \
    -DCMAKE_MAKE_PROGRAM=${HEXAGON_CMAKE_MAKE_PROGRAM} \
    "${HEXAGON_CMAKE_MAKE_FILETYPE}" \
    -DCMAKE_SYSTEM_NAME="Android"  \
    -DOS_TYPE="HLOS" \
    -DOS_VER=${version} \
    -DV=${V}  \
    -DDEEP_CLEAN=${TREE_CLEAN} \
    -DPREBUILT_LIB_DIR=${PREBUILT_LIB_DIR} \
    -DHEXAGON_CMAKE_ROOT=${HEXAGON_CMAKE_ROOT}\
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_build}/ship \
    -DDSP_TYPE=${DSP_TYPE}\
    -DTREE=${1} \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=${MY_BUILD_OPTION}  \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STL=none \
    -DANDROID_NATIVE_API_LEVEL=${android_api_level} \
    -DCMAKE_VERBOSE_MAKEFILE=${verbose} && cd -
}

doUbuntuARM() {
    V="$1"
    UBUNTUARM_BUILD_FULLPATH=${_src}/${V}
    PREBUILT_LIB_DIR=${V//_Debug}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_ReleaseG}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_Release}
    _build=${_src}/${V}

    if [ "$TREE_CLEAN" = "1" ];
    then
        # Delete Projrct Dir
        rm -rf ${_build}
    fi

    DSP_TYPE=CDSP
    if [ "$DOMAIN" = "0" ];
    then
        DSP_TYPE=ADSP
    elif [ "$DOMAIN" = "1" ];
    then
        DSP_TYPE=MDSP
    fi
    echo "Building for domain $DSP_TYPE"

    MY_BUILD_OPTION=Release
    if [ "$BUILD_TYPE" = "Debug" ];
    then
        MY_BUILD_OPTION=Debug
    fi

    ADD_SYMBOLS=0
    if [ "$BUILD_TYPE" = "ReleaseG" ];
    then
        ADD_SYMBOLS=1
    fi

    if [[ -z "${UBUNTUARM_DIR}" ]];
    then
        if [ "$HLOS_VARIANT" = "64" ];
        then
            if [ "$OPENWRT_VARIANT" = "1" ];
            then
                export UBUNTUARM_DIR=${HEXAGON_SDK_ROOT}/tools/OpenWRT64
            else
                export UBUNTUARM_DIR=${HEXAGON_SDK_ROOT}/tools/linaro64
            fi
        else
            if [ "$OPENWRT_VARIANT" = "1" ];
            then
                export UBUNTUARM_DIR=${HEXAGON_SDK_ROOT}/tools/OpenWRT
            else
                export UBUNTUARM_DIR=${HEXAGON_SDK_ROOT}/tools/linaro
            fi
        fi
    fi

    echo "UbuntuARM Toolchain: ${UBUNTUARM_DIR}"
    export UBUNTUARM_TOOLS_DIR=${UBUNTUARM_DIR}

    cd ${_src} && \
    ${CMAKE} \
    -H${_src} \
    -B${_build} \
    -DCMAKE_MAKE_PROGRAM=${HEXAGON_CMAKE_MAKE_PROGRAM} \
    "${HEXAGON_CMAKE_MAKE_FILETYPE}" \
    -DCMAKE_SYSTEM_NAME="UbuntuARM"  \
    -DOS_TYPE="HLOS" \
    -DOS_VER=${version} \
    -DV=${V}  \
    -DOPENWRT=${OPENWRT_VARIANT} \
    -DSOFT_FLOAT=${SOFT_FLOAT_VARIANT} \
    -DDSP_TYPE=${DSP_TYPE}\
    -DDEEP_CLEAN=${TREE_CLEAN} \
    -DADD_SYMBOLS=${ADD_SYMBOLS} \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${_build} \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_build}/ship \
    -DPREBUILT_LIB_DIR=${PREBUILT_LIB_DIR} \
    -DHEXAGON_CMAKE_ROOT=${HEXAGON_CMAKE_ROOT}\
    -DCMAKE_TOOLCHAIN_FILE=${HEXAGON_CMAKE_ROOT}/ubuntuARM${HLOS_VARIANT}_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=${MY_BUILD_OPTION} \
    -DCMAKE_VERBOSE_MAKEFILE=${verbose} && cd -
}

doHexagon() {
    # Hexagon Build
    V=$1
    TARGET=$2
    declare -i qurt_flag
    if [ "$NO_QURT" = "1" ];
    then
        qurt_flag=0
    else
        qurt_flag=1
    fi
    HEXAGON_BUILD=${V}
    HEXAGON_BUILD_FULLPATH=${_src}/${HEXAGON_BUILD}
    PREBUILT_LIB_DIR=${V//_Debug}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_ReleaseG}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_Release}

    _build=${_src}/${V}
    if [ "$TREE_CLEAN" = "1" ];
    then
       # Delete Projrct Dir
       rm -rf ${_build}
    fi

    MY_BUILD_OPTION=Release
    if [ "$BUILD_TYPE" = "Debug" ];
    then
        MY_BUILD_OPTION=Debug
    fi

    ADD_SYMBOLS=0
    if [ "$BUILD_TYPE" = "ReleaseG" ];
    then
        ADD_SYMBOLS=1
    fi

    cd ${_src} && \
    ${CMAKE} \
    -H${_src} \
    -B${_build} \
    -DV=${V} \
    -DOS_VER=${version} \
    -DDEEP_CLEAN=${TREE_CLEAN} \
    -DBUILD_NAME=${TARGET} \
    -DPREBUILT_LIB_DIR=${PREBUILT_LIB_DIR} \
    -DCMAKE_TOOLCHAIN_FILE=${HEXAGON_CMAKE_ROOT}/hexagon_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=${MY_BUILD_OPTION} \
    -DADD_SYMBOLS=${ADD_SYMBOLS} \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_build}/ship \
    -DQURT_OS=${qurt_flag} \
    -DDSP_VERSION=${DSP_VARIANT}\
    -DHEXAGON_CMAKE_ROOT=${HEXAGON_CMAKE_ROOT} \
    -DCMAKE_MAKE_PROGRAM=${HEXAGON_CMAKE_MAKE_PROGRAM} \
    "${HEXAGON_CMAKE_MAKE_FILETYPE}" \
    -DCMAKE_VERBOSE_MAKEFILE=${verbose} && cd -
}

doCustom() {
    V="$1"
    PREBUILT_LIB_DIR=${V//_Debug}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_ReleaseG}
    PREBUILT_LIB_DIR=${PREBUILT_LIB_DIR//_Release}
    _build=${_src}/${V}

    if [ "$HLOS_VARIANT" = 64 ]
    then
        HLOS_NAME=${PREBUILT_LIB_DIR//_aarch64}
    else
        HLOS_NAME=${PREBUILT_LIB_DIR}
    fi

    if [ "$TREE_CLEAN" = "1" ];
    then
        # Delete Projrct Dir
        rm -rf ${_build}
    fi

    DSP_TYPE=CDSP
    if [ "$DOMAIN" = "0" ];
    then
        DSP_TYPE=ADSP
    elif [ "$DOMAIN" = "1" ];
    then
        DSP_TYPE=MDSP
    fi
    echo "Building for domain $DSP_TYPE"

    MY_BUILD_OPTION=Release
    if [ "$BUILD_TYPE" = "Debug" ];
    then
        MY_BUILD_OPTION=Debug
    fi

    ADD_SYMBOLS=0
    if [ "$BUILD_TYPE" = "ReleaseG" ];
    then
        ADD_SYMBOLS=1
    fi

    cd ${_src} && \
    ${CMAKE} \
    -H${_src} \
    -B${_build} \
    -DCMAKE_MAKE_PROGRAM=${HEXAGON_CMAKE_MAKE_PROGRAM} \
    "${HEXAGON_CMAKE_MAKE_FILETYPE}" \
    -DCMAKE_SYSTEM_NAME=${HLOS_NAME}  \
    -DOS_TYPE="HLOS" \
    -DOS_VER=${version} \
    -DV=${V}  \
    -DDSP_TYPE=${DSP_TYPE}\
    -DDEEP_CLEAN=${TREE_CLEAN} \
    -DADD_SYMBOLS=${ADD_SYMBOLS} \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${_build}/ship \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_build}/ship \
    -DPREBUILT_LIB_DIR=${PREBUILT_LIB_DIR} \
    -DHEXAGON_CMAKE_ROOT=${HEXAGON_CMAKE_ROOT}\
    -DCMAKE_TOOLCHAIN_FILE=${HEXAGON_CMAKE_ROOT}/custom_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=${MY_BUILD_OPTION} \
    -DCMAKE_VERBOSE_MAKEFILE=${verbose} && cd -
}

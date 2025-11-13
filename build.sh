#!/bin/bash
KEXT_NAME="AirportRTL8821C"
SDK_PATH="/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
FRAMEWORK_DIR="${SDK_PATH}/System/Library/Frameworks"

SOURCES="RTL8821CPCIe.cpp RTL8821CHAL.cpp AirportRTL8821C.cpp"
OBJECTS="RTL8821CPCIe.o RTL8821CHAL.o AirportRTL8821C.o"
OUTPUT_DIR="build"

KERNEL_HEADER_PATH="${SDK_PATH}/System/Library/Frameworks/Kernel.framework/Headers"
CFLAGS="-arch x86_64 -mmacosx-version-min=13.0 -mkernel -fno-exceptions -fno-rtti -Wno-unused-command-line-argument -DKERNEL -D__kpi_mac -isysroot ${SDK_PATH} -I${KERNEL_HEADER_PATH} -I./include"
LDFLAGS="-v -arch x86_64 -r -Wl,-syslibroot,${SDK_PATH} -lSystem"

rm -rf "${OUTPUT_DIR}"
mkdir -p "${OUTPUT_DIR}/${KEXT_NAME}.kext/Contents/MacOS"

echo "Start Compilation..."
for src in ${SOURCES}; do
    obj="${src%.cpp}.o"
    echo " -> Compile ${src}"
    clang++ ${CFLAGS} -c "${src}" -o "${OUTPUT_DIR}/${obj}"
    
    if [ $? -ne 0 ]; then
        echo "ERROR: Compilation of ${src} failed."
        exit 1
    fi
done

echo "Start Linking..."
clang++ ${LDFLAGS} "${OUTPUT_DIR}"/*.o -o "${OUTPUT_DIR}/${KEXT_NAME}.kext/Contents/MacOS/${KEXT_NAME}"

if [ $? -ne 0 ]; then
    echo "ERROR: Linking failed."
    echo "Tipp: Versuchen Sie, den Linking-Befehl mit der Option '-v' erneut auszuführen, um die Linker-Anweisungen zu überprüfen."
    exit 1
fi

echo "Create Kext-Bundle..."
cp Info.plist "${OUTPUT_DIR}/${KEXT_NAME}.kext/Contents/"

if [ $? -eq 0 ]; then
    echo "✅ Success: Kext created successfully in ${OUTPUT_DIR}/${KEXT_NAME}.kext"
else
    echo "ERROR: Failed to create Kext."
    exit 1
fi
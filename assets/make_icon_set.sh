#!/bin/bash

ICON_FILE=icon_for_mac.png
DIR_NAME=main.iconset

mkdir -p ${DIR_NAME}

sips -z 16 16 ${ICON_FILE} --out ${DIR_NAME}/icon_16x16.png
sips -z 32 32 ${ICON_FILE} --out ${DIR_NAME}/icon_16x16@2x.png
sips -z 32 32 ${ICON_FILE} --out ${DIR_NAME}/icon_32x32.png
sips -z 64 64 ${ICON_FILE} --out ${DIR_NAME}/icon_32x32@2x.png
sips -z 128 128 ${ICON_FILE} --out ${DIR_NAME}/icon_128x128.png
sips -z 256 256 ${ICON_FILE} --out ${DIR_NAME}/icon_128x128@2x.png
sips -z 256 256 ${ICON_FILE} --out ${DIR_NAME}/icon_256x256.png
sips -z 512 512 ${ICON_FILE} --out ${DIR_NAME}/icon_256x256@2x.png
sips -z 512 512 ${ICON_FILE} --out ${DIR_NAME}/icon_512x512.png
sips -z 1024 1024 ${ICON_FILE} --out ${DIR_NAME}/icon_512x512@2x.png

iconutil -c icns ${DIR_NAME}
rm -R ${DIR_NAME}

#!/bin/bash
set -euo pipefail

if [ $# -ne 1 ]; then
    echo "Usage: $0 <linux64|windows64>"
    exit 1
fi

PLATFORM=$1
ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

APP_NAME=$(grep "^appName=" gradle.properties | cut -d'=' -f2)
VERSION=$(grep "^projectVersion=" gradle.properties | cut -d'=' -f2)
JAR="build/libs/$APP_NAME-$VERSION.jar"

JRE_VERSION="17.0.14+7"
JRE_BASE="https://builds.openlogic.com/downloadJDK/openlogic-openjdk-jre/$JRE_VERSION"
PACKR_URL="https://github.com/libgdx/packr/releases/download/4.0.0/packr-all-4.0.0.jar"

case "$PLATFORM" in
    linux64)
        JRE_ARCHIVE="openlogic-openjdk-jre-$JRE_VERSION-linux-x64.tar.gz"
        LAUNCHER="$APP_NAME"
        ;;
    windows64)
        JRE_ARCHIVE="openlogic-openjdk-jre-$JRE_VERSION-windows-x64.zip"
        LAUNCHER="$APP_NAME.exe"
        ;;
    *)
        echo "Error: unknown platform '$PLATFORM'. Use linux64 or windows64."
        exit 1
        ;;
esac

BUILDS="game-builds"
CACHE="$BUILDS/jdk"
JRE_DIR="$CACHE/$PLATFORM"
PACKR="$BUILDS/packr-all-4.0.0.jar"
NAME="$APP_NAME-$VERSION-$PLATFORM"
OUT="$BUILDS/$NAME"

if [ ! -f "$JAR" ]; then
    echo "Error: $JAR is missing; run ./gradlew jar first."
    exit 1
fi

if [ ! -f assets/tiled/tmx/intro.tmx ]; then
    echo "Error: assets/ is missing the game data; run make assets first."
    exit 1
fi

mkdir -p "$CACHE"

if [ ! -f "$PACKR" ]; then
    echo "Downloading packr"
    curl -fL "$PACKR_URL" -o "$PACKR.part"
    mv "$PACKR.part" "$PACKR"
fi

if [ ! -d "$JRE_DIR" ]; then
    echo "Downloading the JRE $JRE_VERSION for $PLATFORM"
    curl -fL "$JRE_BASE/$JRE_ARCHIVE" -o "$CACHE/$JRE_ARCHIVE"
    rm -rf "$JRE_DIR.part"
    mkdir -p "$JRE_DIR.part"
    case "$JRE_ARCHIVE" in
        *.tar.gz) tar -xzf "$CACHE/$JRE_ARCHIVE" -C "$JRE_DIR.part" ;;
        *.zip) unzip -q "$CACHE/$JRE_ARCHIVE" -d "$JRE_DIR.part" ;;
    esac
    rm "$CACHE/$JRE_ARCHIVE"
    mv "$JRE_DIR.part" "$JRE_DIR"
fi

JRE_HOME=$(find "$JRE_DIR" -mindepth 1 -maxdepth 1 -type d | head -n 1)

echo "Building $OUT"
rm -rf "$OUT" "$OUT.zip"
java -jar "$PACKR" \
     --platform "$PLATFORM" \
     --jdk "$JRE_HOME" \
     --useZgcIfSupportedOs \
     --executable "$APP_NAME" \
     --classpath "$JAR" \
     --mainclass mmx.x6.DesktopLauncher \
     --vmargs Xms512M Xmx1024M \
     --resources assets \
     --output "$OUT"

if [ ! -f "$OUT/$LAUNCHER" ]; then
    echo "Error: packr did not make $OUT/$LAUNCHER."
    exit 1
fi

if [ "$PLATFORM" = "linux64" ]; then
    chmod +x "$OUT/$LAUNCHER"
fi

echo "Zipping $NAME"
(cd "$BUILDS" && zip -qr "$NAME.zip" "$NAME")

echo "Done: $OUT and $OUT.zip"

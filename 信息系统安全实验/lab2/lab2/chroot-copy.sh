#!/bin/sh -e
BIN="$1"
DIR="$2"

if [ "$BIN" = "" -o "$DIR" = "" ]; then
    echo "Usage: $0 binary rootdir"
    exit 1
fi

if [ ! -d "$DIR" ]; then
    echo "Error: $DIR does not exist"
    exit 1
fi

if [ ! -x "$BIN" ]; then
    echo "Error: $BIN not executable"
    exit 1
fi

LIBS="`ldd $BIN | grep '=> /' | awk '{print $3;}'`"
LDSO="`ldd $BIN | grep 'ld-linux' | awk '{print $1;}'`"
FILES="$BIN $LIBS $LDSO"
for F in $FILES; do
    D=`dirname $F`
    mkdir -p $DIR/$D
    cp $F $DIR/$F
done


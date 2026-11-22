#! /usr/bin/env bash

set -euo pipefail

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <host> <HDZGOGGLE_binary>"
  exit 1
fi

host="$1"
HDZGOGGLE_binary="$2"

echo "Killing running HDZGOGGLE on $host while uploading new one to /tmp..."
cat $HDZGOGGLE_binary | ssh $host "
  set -x \
  && ps | grep HDZGOGGLE | grep -v grep | awk '{print \$1}' | xargs kill || : \
  && sleep 1 \
  && cat > /tmp/HDZGOGGLE \
  && chmod +wx /tmp/HDZGOGGLE
" || :

echo "starting new HDZGOGGLE..."
ssh $host "LD_LIBRARY_PATH=/lib/eyesee-mpp:/usr/lib/eyesee-mpp /tmp/HDZGOGGLE 2>&1 >/dev/null &"

#! /bin/bash
set -euo pipefail
IFS=$'\n\t'

# Project home page:
# 	http://elm-chan.org/fsw/ff/00index_e.html

# Archives can be downloaded from:
# 	http://elm-chan.org/fsw/ff/arc/*

# RELEASE_VERSION="1"
# RELEASE_VERSION="2"
# RELEASE_VERSION="2a"
# RELEASE_VERSION="3"
# RELEASE_VERSION="3a"
# RELEASE_VERSION="4"
# RELEASE_VERSION="4a"
# RELEASE_VERSION="4b"
# RELEASE_VERSION="5"
# RELEASE_VERSION="5a"
# RELEASE_VERSION="6"
# RELEASE_VERSION="7a"
# RELEASE_VERSION="7c"
# RELEASE_VERSION="7e"
# RELEASE_VERSION="8"
# RELEASE_VERSION="8a"
# RELEASE_VERSION="8b"
# RELEASE_VERSION="9"
RELEASE_VERSION="9a"
RELEASE_ID="R0.0${RELEASE_VERSION}"

NAME_PREFIX="ff"
RELEASE_NAME="${NAME_PREFIX}${RELEASE_VERSION}"
ARCHIVE_NAME="${RELEASE_NAME}.zip"
ARCHIVE_URL="http://elm-chan.org/fsw/ff/arc/${ARCHIVE_NAME}"

LOCAL_ARCHIVE_FILE="/tmp/xpacks/${ARCHIVE_NAME}"

for f in *
do
  if [ "${f}" == "scripts" ]
  then
    :
  else
    rm -rf "${f}"
  fi
done

if [ ! -f "${LOCAL_ARCHIVE_FILE}" ]
then
  mkdir -p $(dirname ${LOCAL_ARCHIVE_FILE})
  curl -o "${LOCAL_ARCHIVE_FILE}" -L "${ARCHIVE_URL}"
fi

unzip -q "${LOCAL_ARCHIVE_FILE}"

# For just in case...
find . -name '*.exe' -exec rm -v \{} \;

cat <<EOF >README.md
# Chan FatFs - Generic FAT File System Module

This project, hosted on [GitHub](https://github.com/xpacks),
includes the Chan FAT FS files, plus a POSIX layer adapter.

## Version

* ${RELEASE_ID}

## Documentation

The Chan FAT FS main site is
http://elm-chan.org/fsw/ff/00index_e.html.

## Original files

The original files are stored in the \`originals\` branch.

These files were extracted from \`${ARCHIVE_NAME}\`.

To save space, the following folders/files were removed:

* (none)

EOF

echo
echo Check if ok and when ready, issue: \`git commit -m ${ARCHIVE_NAME}\`
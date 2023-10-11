[![GitHub issues](https://img.shields.io/github/issues/xpacks/chan-fatfs.svg)](https://github.com/xpacks/chan-fatfs/issues/)
[![GitHub pulls](https://img.shields.io/github/issues-pr/xpacks/chan-fatfs.svg)](https://github.com/xpacks/chan-fatfs/pulls)

# Maintainer info

## Project repository

The project is hosted on GitHub:

- <https://github.com/xpacks/chan-fatfs.git>

To clone the stable branch (`xpack`), run the following commands in a
terminal (on Windows use the _Git Bash_ console):

```sh
rm -rf ~/Work/xpack-3rd-party/chan-fatfs.git && \
mkdir -p ~/Work/xpack-3rd-party && \
git clone \
  https://github.com/xpacks/chan-fatfs.git \
  ~/Work/xpack-3rd-party/chan-fatfs.git
```

For development purposes, clone the `xpack-develop` branch:

```sh
rm -rf ~/Work/xpack-3rd-party/chan-fatfs.git && \
mkdir -p ~/Work/xpack-3rd-party && \
git clone \
  --branch xpack-develop \
  https://github.com/xpacks/chan-fatfs.git \
  ~/Work/xpack-3rd-party/chan-fatfs.git
```

## Prerequisites

A recent [xpm](https://xpack.github.io/xpm/), which is a portable
[Node.js](https://nodejs.org/) command line application.

## How to make new releases

### Release schedule

There are no fixed releases, the project aims to follow the upstream releases.

### Check Git

In the `xpacks/chan-fatfs` Git repo:

- switch to the `xpack-develop` branch
- if needed, merge the `xpack` branch

No need to add a tag here, it'll be added when the release is created.

### Increase the version

Determine the upstream version (like `0.13.1`)

- <http://elm-chan.org/fsw/ff/00index_e.html>

Update the`package.json` file; add an extra digit in the
pre-release field, and initially also add `.pre`,
for example `0.13.1-4.pre.1`.

### Fix possible open issues

Check GitHub issues and pull requests:

- <https://github.com/xpacks/chan-fatfs/issues/>

and fix them; assign them to a milestone (like `0.13.1-4`).

### Update `README-MAINTAINER-XPACK.md`

Update the `README-MAINTAINER-XPACK.md` file to reflect the changes
related to the new version.

### Update `CHANGELOG-XPACK.md`

- open the `CHANGELOG-XPACK.md` file
- check if all previous fixed issues are in
- add a new entry like _* v0.13.1-4_
- commit with a message like _prepare v0.13.1-4_

### Push changes

- commit and push

### Commit the new version

- select the `xpack-develop` branch
- commit all changes
- `npm pack` and check the content of the archive, which should list
  only `package.json`, `README.md`, `LICENSE`, `CHANGELOG-XPACK.md`,
  the `doxygen-awesome-*.js` and `doxygen-custom/*` files;
  possibly adjust `.npmignore`
- `npm version 0.13.1-4`
- push the `xpack-develop` branch to GitHub
- the `postversion` npm script should also update tags via `git push origin --tags`

### Update the repo

When the package is considered stable:

- with a Git client (VS Code is fine)
- merge `xpack-develop` into `xpack`
- push to GitHub
- select `xpack-develop`

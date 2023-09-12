[![Packaging status](https://repology.org/badge/tiny-repos/dcfldd.svg)](https://repology.org/project/dcfldd/versions)

# dcfldd -- enhanced version of `dd` for security and forensics.

`dcfldd` is an enhanced version of the `dd` command with additional features
useful for forensics and information security, designed to run on Unix or
Unix-like (POSIX-compliant) operating systems.

## Table of Contents

- [Introduction](#introduction)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Obtaining the source](#obtaining-the-source)
  - [Building](#building)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

dcfldd was originally created by Nicholas Harbour from the DoD Computer Forensics
Laboratory (DCFL) as a fork of the GNU coreutils `dd` program. He maintained
the package long after his affiliation with the DCFL had ended.  The original
project became stalled circa 2006, with no further updates on the original
homepage[^1] since then.

[^1]: https://sourceforge.net/projects/dcfldd

Since 2019, dcfldd has been maintained by volunteers under the [Ressurecting
Open Source Projects](https://github.com/resurrecting-open-source-projects)
organization on Github. The first version released by the organization was 1.5,
based on the original 1.3.4-1 version with patches applied from the Debian
dcfldd package[^2], along with some improvements.

[^2]: https://tracker.debian.org/pkg/dcfldd

Some key features distinguish dcfldd from coreutils dd:

  - **Hashing on-the-fly**: dcfldd can hash the input data as it is being
    transferred, helping to ensure data integrity.
  - **Status output**: dcfldd can update the user of its progress in terms of the
    amount of data transferred and how much longer operation will take.
  - **Flexible disk wipes**: dcfldd can be used to wipe disks quickly and with a
    known pattern if desired.
  - **Image/wipe verify**: dcfldd can verify that a target drive is a bit-for-bit
    match of the specified input file or pattern.
  - **Multiple outputs**: dcfldd can output to multiple files or disks at the same
    time.
  - **Split output**: dcfldd can split output to multiple files with more
    configurability than the split command.
  - **Piped output and logs**: dcfldd can send all its log data and output to
    commands as well as files natively.

Several operating systems already package and distribute dcfldd[^3], although
some are still stuck on the ancient last version from the original project
homepage.

[^3]: https://repology.org/project/dcfldd/versions

Users of Debian and derivative distributions can install dcfldd through APT:


```console
user@host:~$ sudo apt install dcfldd
```

For other operating systems, please consult the respective package manager information.

## Getting Started

dcfldd is a C language project that relies on GNU Autotools to create its
build system (a "GNU Build System"). To avoid polluting the source tree with
files that can be easily regenerated, the dcfldd repository does not contain a
build system, only the required files to create one by using Autotools.

On the other hand, release tarballs are generated from the source control tree
by using `make distcheck`. As such, those tarballs already have a build system
ready to be used, and it users are not required to install Autotools.

Take this into account when you choose the way you will obtain the source code.
End-users might be better using release tarballs to build dcfldd, as it
requires less dependencies and tools. Potential contributors should familiarize
themselves with the git repository and the associated workflow.

### Prerequisites

There are no runtime dependencies for this package, except for `libc` (which is
already present in all POSIX-like operating systems). During build, dcfldd
build depends on the following packages:

- Several GNU Autotools utilities (only when using source from git)
- ANSI C compiler
- GNU Make
- C POSIX library ("libc") development files
- txt2man (used to generate dcfldd man page)

On Debian systems, installing the packages below will recursively install the
other required packages:

- `autoconf`
- `gcc`
- `make`
- `txt2man`

You can install them via APT:

```shell
apt install autoconf gcc make txt2man
```

Remember that you may omit installing `autoconf` if you are installing from the
release tarballs. For other operating systems or package managers, please refer
to their documentation to find out how to install the required packages.

### Obtaining the source

dcfldd source code can be obtained from source control by using `git`:

```shell
git clone https://github.com/resurrecting-open-source-projects/dcfldd.git
```

Alternatively, you can download the latest tarball from the [Releases](https://github.com/resurrecting-open-source-projects/dcfldd/releases) page.
Then, unpack it to a directory:

```shell
tar xvzf dcfldd-?.?.tar.gz --strip-components 1 --one-top-level=dcfldd
```

Then change the current working directory to the dcfldd source root:

```shell
cd dcfldd
```

### Building

If you cloned the repository from source control, you will need to generate the build system first:

```shell
./autogen.sh
```

After you have generated the build system (or if it is already present on the source tree), we can configure and run the build, and install dcfldd:

```shell
# Default installation path prefix is /usr/local.
# Use ./configure --prefix=$PREFIX to install in $PREFIX

./configure
make
sudo make install
```

## Usage
TBD.

## Contributing

dcfldd needs your help. **If you are a programmer** and if you want to help a
nice project, this is your opportunity.

If you are interested to help dcfldd, read the project [contributing guidelines](CONTRIBUTING.md).

## License

Licensed under the terms of the GNU General Public License version 2 or later.
For more information, refer to the [accompanying license information](COPYING).

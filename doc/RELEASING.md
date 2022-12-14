# Releasing dcfldd

This document covers the steps needed to release dcfldd.

## Version number

dcfldd adheres to [Semantic Versioning (SemVer)](https://semver.org/) 2.0. As
such, we should use the `MAJOR.MINOR.PATCH` format for version numbers, and
increment the:

1. MAJOR version when we make incompatible user-interface changes.
2. MINOR version when we add functionality in a backwards compatible manner.
3. PATCH version when we make backwards compatible bug fixes.

By using the [Conventional Commits](https://www.conventionalcommits.org/)
specification, we will be able to automatically determine the right version to
be attributed to the release. Git hooks and other tools can be used to enforce
compliant commits.

## Run the `release.sh` script

This script will:

- update the version on `configure.ac`
- generate the manpage with right version and release date
- commit the version and date changes
- tag the HEAD commit with the right version

## TODO: review below and add the appropriate steps on the doc
When updating, change the following files (if needed):

- Update rights.
- Update ChangeLog.
- man/create-man.sh (DATE, version)
- Generate a new manpage.
- Check for spelling errors in ChangeLog, manpage and README.
- Check final manpage with man command.
- Update README.
- Update configure.ac (VERSION)
- Test in Debian Sid.
- Release extra files with make dist.

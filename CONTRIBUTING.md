# Contributing to dcfldd

First of all, thanks for your interest in dcfldd development. In general, there are two ways to contribute to the project:

- Reporting bugs
- Collaborating with Pull Requests

In this guide you will get an overview of the contribution worflow from opening
an issue, creating a PR, reviewing and merging the PR.


## Getting Started

Contributions are made to this repository via Issues and Pull Requests (PRs). A
few guidelines that cover both:

- Ensure you are using the latest dcfldd development version. Your issue may
  already have been solved on master.

- If you are using a version of dcfldd packaged by your operating system or
  distribution, discuss your contributions there first, as there might be
  differences from the upstream version. Package maintainers will forward the
  bug reports or patches, if the deem it adequate.

- Search for [existing Issues](https://github.com/resurrecting-open-source-projects/dcfldd/issues)
  and [PRs](https://github.com/resurrecting-open-source-projects/dcfldd/pulls)
  before creating your own.

### Issues

Issues should be used to report problems dcfldd, request a new feature, or to
discuss potential changes before a PR is created. When you create a new Issue,
a template will be loaded that will guide you through collecting and providing
the information we need to investigate.

If you find an Issue that addresses the problem you're having, please add your
own reproduction information to the existing issue rather than creating a new
one.

If a related issue does not exist, you can open a new issue using the
[appropriate form](https://github.com/resurrecting-open-source-projects/dcfldd/issues/new/choose).


### Pull Requests

PRs to dcfldd are always welcome and can be a quick way to get your fix or improvement slated for the next release.
As PRs are meant to be reviewed before being merged in the codebase, in general they should:

- Each PR should have only one concern (either fix a bug, or implement a feature, for example, not both).
- Implement only the minimal changes related to that concern.
- Be composed by one or more commits that implement those changes.
- Have one commit per logical unit of change.
- Each commit should be small in scope: it should do only one thing. Although this often correlates to minimizing the modified lines of code, it is not a firm requirement.
- Each commit should be atomic (a stable, independent unit of change). This means that the codebase should still build, pass tests, and function if rolled back to that exact commit.

- Each commit should leave the codebase in a buildable, consistent state.

- Be aligned to project vision, principles etc. If in conflict, a discussion should be opened before.
Each PR should address a single logical change. If you want to address multiple logical changes, open a PR per change.

Think of your PR as a story. Your commits should compose the narrative of how your story was written in a logical sequence. This makes it easier to review your changes.


In general, the workflow used to contribute to dcfldd is described in [GitHub
flow](https://docs.github.com/en/get-started/quickstart/github-flow).


You should create a new branch for each pull request you intend to send.  Any
changes you make should be committed to this branch. No changes should be
committed directly on master.

When making your changes, please take into account the coding style, project goals, guidelines, etc.

### Notes about commits

Before committing your changes, make sure your local copy builds and works.

Each commit in the context of a pull request must be an _atomic commit_. This means that each commit addresses only the changes for a single, complete unit of work.

As an example, if your pull request is about adding a new feature, you might end up with:

- a commit that changes the required source files to implement the feature;
- a second commit that adds a test to catch regressions; and
- a third commit that explains the feature on the documentation.

Notice that there is no problem for a commit to modify several files at once, but those changes should be related. This is done to ease review.

Ideally, each commit must leave the source tree in a buildable and working state. Of course, you are free to make any commits you want on your local copy, but if they do not comply with this requirement, you must rebase before sending your PR.

Only rebase changes that have not landed on master.

Your commit message must comply to the following format:

- a subject line, in the imperative ("Add feature X", "Fix wrong Y", "Make dcfldd...")

Commit messages shall be divided in two parts:

- A mandatory summary line (50 chars max)
- An optional message body (each line 72 char max).

The message body paragraphs should be separated from themselves, and from the
summary line by one blank line.

The summary line should not exceed 50 lines. `git log` shows summary lines with 4 spaces padding on the left. If we consider a 80-column terminal, we will want 4 spaces on the right as padding to keep the alignment, thus ending up with an useful estate of 72 columns.

For the subject line, lthough 50 columns is not a hard limit, when using `git log` this helps in making the subject line not passing the commit SHA-1 line in length, which would otherwise make it hard to distinguish from the message body.


#### Signing-off commits
Any non-trivial commit should be signed-off.

By contributing to this project, you agree to the [Developer Certificate of
Origin (DCO)](https://developercertificate.org/).

The DCO is a lightweight way for contributors to certify that they wrote or otherwise have the right to submit the code to the project.

Contributors _sign-off_ that they adhere to these requirements by adding a `Signed-off-by` line to commit messages.

```
This is my commit message

Signed-off-by: Random J Developer <random@developer.example.org>
```

Git even has a `-s` command line option to append this automatically to your commit message:

```console
$ git commit -s -m 'This is my commit message'
```

### Push your branch to your remote
When your work is done:

- check if the project master was updated
- if it was, rebase on master and test your changes again
- push your branch to your remote

### Create a new Pull Request

The PR description should not be the same text as the auto-generated commit log.

If the PR fixes an issue, write "Closes: #X" on the end of the commit message body.

- When writing the PR description, use the conventional commit guidelines.

=======
First of all, thanks for your interest in dcfldd development. Contributions are
made to this repository via Issues and Pull Requests (PRs). Before opening a
new issue or pull request, please:

- Ensure you are using the latest dcfldd development version. Your issue may
  already have been solved on master.

- If you are using a version of dcfldd packaged by your operating system or
  distribution, discuss your contributions there first, as there might be
  differences from the upstream version. Package maintainers will forward the
  bug reports or patches, if the deem it adequate.

- Search for [existing Issues](https://github.com/resurrecting-open-source-projects/dcfldd/issues)
  and [PRs](https://github.com/resurrecting-open-source-projects/dcfldd/pulls)
  before creating your own.

## Issues

Issues should be used to report problems dcfldd, request a new feature, or to
discuss potential changes before a PR is created. When you create a new Issue,
a template will be loaded that will guide you through collecting and providing
the information we need to investigate.

If you find an Issue that addresses the problem you're having, please add your
own reproduction information to the existing issue rather than creating a new
one.

If a related issue does not exist, you can open a new issue using the
[appropriate form](https://github.com/resurrecting-open-source-projects/dcfldd/issues/new/choose).


## Pull Requests

PRs to dcfldd are always welcome and can be a quick way to get your fix or improvement slated for the next release.

Think of your PR as a story. Your commits should compose the narrative of how your story was written in a logical sequence. This makes it easier to review your changes.

Each pull request should address a single concern in the codebase. It may either add functionality, or fix a bug for example, but not both.
Sometimes, addressing a single concern may require changes in distinct areas of the codebase.
For example, to add functionality, you might need to:

- Change existing functions to accomodate the new use case.
- Implement the new functionality upon the changed functions.
- Add online help for the new functionality.
- Document the new functionality.
- Add tests for the new functionality.

To that end, each PR should be composed by one or more _small_ and _atomic_ commits that together address the PR concern as a whole.

A commit is small when it makes a single logical unit of change that advances the codebase one step closer to the end state proposed by the PR, with subsequent small commits building upon it.

An commit is atomic when it is a stable, independent unit of change. This means that an atomic commit should leave the codebase in a consistent state (buildable, testable and functioning), without depending on posterior commits.

- Be aligned to project vision, principles etc. If in conflict, a discussion should be opened before.

In general, the workflow used to contribute to dcfldd is described in [GitHub
flow](https://docs.github.com/en/get-started/quickstart/github-flow).


### Create a topic branch to work on

Before working on a pull request, you should create a branch in your local repository with a short, descriptive name.
No 
You should create a new branch for each pull request you intend to send.  Any
changes you make should be committed to this branch. No changes should be
committed directly on master.

When making your changes, please take into account the coding style, project goals, guidelines, etc.

### Notes about commits

Before committing your changes, make sure your local copy builds and works.

Only rebase changes that have not landed on master.


### Commit message guidelines

Commit messages should describe  _what_ changed and _why_, not _how_. The _how_ part is the diff.

Commit messages shall be divided in two parts:

- A mandatory summary line (50 chars max).
- An optional message body (each line 72 char max).

The message body paragraphs should be separated from themselves, and from the
summary line by one blank line.

The rationale for not exceeding 72 chars is that `git log` does not do any
special wrapping of the commit messages. It only indents each line with 4
spaces on the left.  With the default pager of `less -S`, this means paragraphs
will flow far off the edge of the screen. To keep consistency and symmetry, we
consider a minimum 80-column terminal as our target, and wrap on 72 chars to
keep 4 spaces on the right.

For the summary line, although 50 columns is not a hard limit, when using `git
log` this helps in making the subject line not passing the commit SHA-1 line in
length, which would otherwise make it hard to distinguish from the message
body.

```
Capitalized, short (50 chars or less) summary

More detailed explanatory text, if necessary.  Wrap it to about 72
characters or so.  In some contexts, the first line is treated as the
subject of an email and the rest of the text as the body.  The blank
line separating the summary from the body is critical (unless you omit
the body entirely); tools like rebase can get confused if you run the
two together.

Write your commit message in the imperative: "Fix bug" and not "Fixed bug"
or "Fixes bug."  This convention matches up with commit messages generated
by commands like git merge and git revert.

Further paragraphs come after blank lines.

- Bullet points are okay, too

- Typically a hyphen or asterisk is used for the bullet, followed by a
  single space, with blank lines in between, but conventions vary here

- Use a hanging indent
```

### Signing-off commits
Any non-trivial commit should be signed-off.

By contributing to this project, you agree to the [Developer Certificate of
Origin (DCO)](https://developercertificate.org/).

The DCO is a lightweight way for contributors to certify that they wrote or otherwise have the right to submit the code to the project.

Contributors _sign-off_ that they adhere to these requirements by adding a `Signed-off-by` line to commit messages.

```
This is my commit message

Signed-off-by: Random J Developer <random@developer.example.org>
```

Git even has a `-s` command line option to append this automatically to your commit message:

```console
$ git commit -s -m 'This is my commit message'
```

### Push your branch to your remote
When your work is done:

- check if the project master was updated
- if it was, rebase on master and test your changes again
- push your branch to your remote

### Create a new Pull Request

The PR description should not be the same text as the auto-generated commit log.

If the PR fixes an issue, write "Closes: #X" on the end of the commit message body.

- When writing the PR description, use the conventional commit guidelines.

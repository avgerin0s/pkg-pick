Developing
==========

Setup
-----

To build from a fresh checkout:

    ./autogen.sh
    ./configure
    make

Editing
-------

Use these Vim settings for the correct indention and formatting:

```
setlocal sw=0 ts=8 noet
setlocal cinoptions=:0,t0,+4,(4
```

Release
-------

1. Update the version in `configure.ac`:

        AC_INIT([pick], [0.0.2], [hello@thoughtbot.com])

2. Verify the tarball:

        make distcheck
        tar -ztf pick-0.0.2.tar.gz | less

3. Tag the repo:

        git checkout master
        git pull --rebase
        git push origin master
        git status
        git tag --sign -m "v0.0.2" v0.0.2
        git push origin master --tags

4. Sign the tarball:

        gpg -sab pick-0.0.2.tar.gz

5. Verify the signature:

        gpg --verify pick-0.0.2.tar.gz.asc pick-0.0.2.tar.gz

6. [Announce the release on
   GitHub](https://github.com/thoughtbot/pick/releases/new).

7. Release Homebrew package by running:

        make release

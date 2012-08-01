Mirage kFreeBSD Backend
=======================

This tiny project is about putting [Mirage](http://openmirage.org/) to
the FreeBSD kernel space for evaluation.  Contents of this repository
currently is as follows.


Warning
-------

*Please do note that this is a work-in-progress effort, which will work
only on the `amd64` architecture with a recent FreeBSD -CURRENT.  In
addition, some of the original features may not be yet implemented fully
at all.*


Prerequisites
-------------

In order to be able to build the sources, one has to install the OCaml
compiler from the Ports Collection.  (It has been tested with version
3.12.1 so far.)

    # cd /usr/ports/lang/ocaml
    # make install clean

It is also required to install the findlib utility.

    # cd /usr/ports/devel/ocaml-findlib
    # make install clean


Installing the Customized Standard Library
------------------------------------------

There is a customized standard library (which sources were ripped of the
3.12.1 OCaml distribution) to be built before one could work with the
sources.  It is in the following directory.

    packages/mirage-stdlib

It contains a standalone version of the modified sources that could be
compiled using BSD make.  It is possible to specify the place of the
used OCaml toolchain by the `PREFIX` variable.  By default, that is
`/usr/local`.

    # make install clean

This command will install the `mirage-stdlib` package that all the
others rely on.


Installing the Rest of the Packages
-----------------------------------

The other OCaml packages available are as follows.

    packages/cstruct
    packages/lwt
    packages/mirage-platform

All of them will be needed to build Mirage's OS module, which is then
required to build a FreeBSD kernel module. Packages `cstruct` and `lwt`
are stripped and adapted versions of the corresponding packages, while
`mirage-platform` implements the internals of the `OS` module.

Each of them can be installed from its directory with a single command
(as root):

    # make install clean


Creating and Loading An Application as Kernel Module
----------------------------------------------------

There is also a sample test application added in the following
directory.

    packages/mirage-test

It does not require to be installed, it is enough to build it (as root).

    # make

This should generate a kernel module under the name `mirage.ko`, which
then could be loaded into the running kernel.

**Warning: It is not guaranteed that the module will behave as expected.
Loading the module may result in a crash and, as a side effect, in data
loss.  Do not try this on a production machine ever.  You have been
warned!**

Note that since the kernel module is not installed to the default module
path, one must specify it by the name of the file.

    # kldload ./mirage.ko

Upon the successful loading, some sysctl(3) variables should be created
under the MIB `kern.mirage`.

    kern.mirage.debug

Set the verbosity of debug information provided by the module.  It is
`3` (the highest) by default -- set to `0` if no information is needed.

    kern.mirage.run

Start the code inside the kernel module.  Writing any value to this
variable will trigger the initialization routine and launches the
execution of the embedded OCaml program.  Note that a program may be
restarted many times, however this is not recommended -- reload the
module first instead.

    # sysctl kern.mirage.run=1


Contact & Contribute
--------------------

If there are patches, comments, problem reports or any other feedback,
feel free to post them to the
[Mirage mailing list](https://lists.cam.ac.uk/mailman/listinfo/cl-mirage).

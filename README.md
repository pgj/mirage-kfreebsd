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


Ports
-----

There are FreeBSD ports in the following directories.

    ports/lang/ocaml
    ports/lang/ocaml-kern

They contain patches to the OCaml compiler to build kernel-enabled
programs.  Here, `ocaml-kern` is a slave port to the standard `ocaml`
port.  This will install the `ocaml-kern` package that may co-exist with
the original ocaml installation, however it should be installed with a
different `PREFIX`.

That is, as a command, for example:

    # make install clean PREFIX=/usr/local/okaml


Packages
--------

There are also BSD make-based OCaml packages in the following
directories.

    packages/cstruct
    packages/lwt
    packages/mirage-platform

They contain all the packages that will be needed to build Mirage's OS
module, which is then required to build a FreeBSD kernel module.
Packages `cstruct` and `lwt` are stripped and BSD makefied versions of
the corresponding packages, while `mirage-platform` implements the
internals of the `OS` module.

Each of them can be installed from its directory with a single command
(as root):

    # make KAMLROOT=/usr/local/okaml install clean

`KAMLROOT` sets the path where the kernel-enabled version of the OCaml
compiler was installed previously.  These packages will be installed to
this path, under the `lib/ocaml/site-lib` directory.  That is, they are
kept separately from the regular OCaml packages this way.


Creating and Loading An Application as Kernel Module
----------------------------------------------------

There is also a sample test application added in the following
directory.

    packages/mirage-test

It does not require to be installed, it is enough to build it (as root).

    # make KAMLROOT=/usr/local/okaml

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

# Node native module error example

This repository provides a minmal reproduction of an error that was initially
encountered when running [Parcel][parcel] inside an [Alpine Linux][alpine]
[podman][podman] container. It loads and starts a native node module, that
initiates a [`poll`][poll] system call, and then loads another native module,
that does nothing. This causes the first native module to crash.

[parcel]: https://parceljs.org/
[alpine]: https://alpinelinux.org/
[podman]: https://podman.io/
[poll]: https://linux.die.net/man/2/poll

## The problem

The error is caused by the [musl libc implementation][musl] sending [SIGNAL
34][musl-signal] signals to all threads when failing to make a
`MEMBARRIER_CMD_PRIVATE_EXPEDITED` membarrier syscall in its [`__membarrier`
function][membarrier]. This causes the [parcel watcher][parcel-watcher] to be
interruped while [polling][parcel-polling], causing it to throw an error and
exit.

[musl]: https://musl.libc.org/
[musl-signal]: https://git.musl-libc.org/cgit/musl/tree/src/internal/pthread_impl.h#n131
[membarrier]: https://git.musl-libc.org/cgit/musl/tree/src/linux/membarrier.c?h=master#n22
[parcel-watcher]: https://github.com/parcel-bundler/watcher
[parcel-polling]: https://github.com/parcel-bundler/watcher/blob/9adac36b0c0a8f5107266f8a843dc395cd7c4fc6/src/linux/InotifyBackend.cc#L38

This membarrier call is caused by another native module being loaded. Note that
in this repository the [declaration of a `thread_local` variable][thread-local]
is required for the error to occur, presumably because it causes the binary to
be linked against the pthread library or so.

[thread-local]: interrupt/interrupt.cc#L5

The failure of the `MEMBARRIER_CMD_PRIVATE_EXPEDITED` membarrier syscall seems
to be rooted in podmans default [Seccomp BPF][seccomp] policy. [Disabling
seccomp confinement][seccomp-disable] in podman will avoid the error.

[seccomp]: https://www.kernel.org/doc/html/v4.16/userspace-api/seccomp_filter.html
[seccomp-disable]: https://docs.podman.io/en/latest/markdown/podman-run.1.html#security-opt-option

## Building and executing

The error can be reproduced by building this project with `podman build -t
node-error-reproduction .` and running it with `podman run --rm
node-error-reproduction`. Running it with seccomp disabled with `podman run
--rm --security-opt seccomp=unconfined node-error-reproduction` will avoid the
error and cause the program to wait indefinitely.

This was tested in a Debian bullseye virtual machine with podman version 3.0.1.

## The solution

The parcel watcher could be fixed by either masking signal 34 while polling or
by recovering from EINTR errors instead of crashing. Masking the signal could
be accomplished by replacing `poll` with [`ppoll`][ppoll] and using the sigmask
argument.

[ppoll]: https://linux.die.net/man/2/ppoll

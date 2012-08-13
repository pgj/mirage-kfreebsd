#!/usr/sbin/dtrace -s

#pragma D option quiet

mirage:kernel:io_page:refcount
{
    printf("%s:%s(%p,%d)\n", probefunc, probename, args[0], args[1]);
}

fbt:kernel:contigmalloc:entry
{
    printf("%s:%s(%d)\n", probefunc, probename, args[0]);
}

fbt:kernel:contigmalloc:return
{
    printf("%s:%s = %p\n", probefunc, probename, args[0]);
}

fbt:kernel:contigfree:entry
{
    printf("%s:%s(%p,%d)\n", probefunc, probename, args[0], args[1]);
}

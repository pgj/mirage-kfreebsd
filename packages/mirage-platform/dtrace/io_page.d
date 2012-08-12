#!/usr/sbin/dtrace -s

#pragma D option quiet

mirage:kernel:io_page:refcount
{
    printf("%s:%s(%p,%d)\n", probefunc,probename, arg0, arg1);
}

mirage:kernel:io_page:contigmalloc
{
    printf("%s:%s(%p,%d)\n", probefunc,probename, arg1, arg0);
}

mirage:kernel:io_page:contigfree
{
    printf("%s:%s(%p,%d)\n", probefunc, probename, arg0, arg1);
}

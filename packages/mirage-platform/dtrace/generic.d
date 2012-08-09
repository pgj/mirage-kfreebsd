#!/usr/sbin/dtrace -s

#pragma D option quiet

mirage:kernel:kthread:entry
{
    printf("-> %s()\n", probefunc);
}

mirage:kernel:kthread:return
{
    printf("<- %s()\n", probefunc);
}

mirage:kernel:caml_startup:start
{
    printf("kernel thread activated, caml run-time starts\n");
}

mirage:kernel:caml_startup:finish
{
    printf("caml run-time finished\n");
}

mirage:kernel:kthread_loop:start
{
    printf("main function found, kicking off the main loop\n");
}

mirage:kernel:kthread_loop:stop
{
    printf("main loop exited: (%d,%d)\n", arg0, arg1);
}

mirage:kernel:kthread_init:entry
{
    printf("-> mirage_kthread_init()\n");
}

mirage:kernel:kthread_init:return
{
    printf("<- mirage_kthread_init()\n");
}

mirage:kernel:kthread_deinit:entry
{
    printf("-> mirage_kthread_deinit()\n");
}

mirage:kernel:kthread_deinit:return
{
    printf("<- mirage_kthread_deinit()\n");
}

mirage:kernel:kthread_launch:entry
{
    printf("-> mirage_kthread_launch()\n");
}

mirage:kernel:kthread_launch:return
{
    printf("<- mirage_kthread_launch()\n");
}

mirage:kernel:block_kernel:entry
{
    printf("blocking kernel for %d us\n", arg0);
}

mirage:kernel:block_kernel:return {}

mirage:kernel:alloc_pages:entry
{
    printf("-> %s(%d)\n", probefunc, arg0);
}

mirage:kernel:alloc_pages:return
{
    printf("<- %s()\n", probefunc);
}

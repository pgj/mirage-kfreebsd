#!/usr/sbin/dtrace -s

#pragma D option quiet

fbt:mirage:mirage_kthread_body:entry
{
    printf("-> %s()\n", probefunc);
}

fbt:kernel:kthread_exit:entry
{
    printf("<- mirage_kthread_body()\n");
}

fbt:mirage:caml_startup:entry
{
    printf("kernel thread activated, caml run-time starts\n");
}

mirage:kernel:kthread_loop:start
{
    printf("main function found, kicking off the main loop\n");
}

mirage:kernel:kthread_loop:stop
{
    printf("main loop exited: (%d,%d)\n", arg0, arg1);
}

mirage:kernel:block:timeout
{
    printf("blocking kernel for %d us\n", arg0);
}

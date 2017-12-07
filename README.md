This module test an issue with patch 'Protect key list during fork()' (rhbz#1268226) while using PyGILState_GetThisThreadState in the prepare callback of pthread_atfork.

This issue has been seen on centos7 with python-2.7.5-58.el7.x86_64.rpm.

How to reproduce the bug:
* Compile Python2.7 with Charalampos Stratakis's patch (https://github.com/python/cpython/pull/783).
* Compile this module: `python2.7 setup.py build_ext -i`
* Execute a command that will perform a fork: `python2.7 -c "import test_fork_hanging; import subprocess; subprocess.check_output('ls')"`
* Python process will hang forever.

Here's below the complete backtrace:
```gdb
(gdb) thread apply all bt

Thread 1 (Thread 0x7f11a0b6b700 (LWP 11424)):
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
#1  0x00000000004f7be5 in PyThread_acquire_lock (waitflag=<optimized out>, lock=<optimized out>) at Python/thread_pthread.h:324
#2  find_key (key=1, value=value@entry=0x0) at Python/thread.c:268
#3  0x00000000004f816b in PyThread_get_key_value (key=<optimized out>) at Python/thread.c:360
#4  0x00007f119fd62fb2 in __libc_fork () at ../nptl/sysdeps/unix/sysv/linux/x86_64/../fork.c:95
#5  0x00007f11a076d3c5 in __fork () at ../nptl/sysdeps/unix/sysv/linux/pt-fork.c:25
#6  0x0000000000501926 in posix_fork (self=<optimized out>, noargs=<optimized out>) at ./Modules/posixmodule.c:3914
#7  0x00000000004b96fc in call_function (oparg=<optimized out>, pp_stack=<optimized out>) at Python/ceval.c:4341
#8  PyEval_EvalFrameEx (f=0xdf0da0, throwflag=128) at Python/ceval.c:2994
#9  0x00000000004bb38b in PyEval_EvalCodeEx (co=0x7f11a0a2ea30, globals=0x80, locals=0x0, args=0xb, argcount=0,
    kws=0x501910 <posix_fork>, kwcount=2, defs=0x0, defcount=0, closure=0x0) at Python/ceval.c:3589
#10 0x00000000004b93eb in fast_function (nk=<optimized out>, na=<optimized out>, n=<optimized out>, pp_stack=<optimized out>,
    func=<optimized out>) at Python/ceval.c:4452
#11 call_function (oparg=<optimized out>, pp_stack=<optimized out>) at Python/ceval.c:4377
#12 PyEval_EvalFrameEx (f=0xdf0da0, throwflag=128) at Python/ceval.c:2994
#13 0x00000000004bb38b in PyEval_EvalCodeEx (co=0x7f11a0a17cb0, globals=0x80, locals=locals@entry=0x0, args=0x2, argcount=0,
    kws=0x501910 <posix_fork>, kws@entry=0x7f11a0a03338, kwcount=1, defs=0x7f11a0a309c8, defcount=13, closure=0x0)
    at Python/ceval.c:3589
#14 0x000000000052c0bf in function_call (func=0x7f119e838c80, arg=0x7f11a0a030e0, kw=0x7f119e83b168) at Objects/funcobject.c:523
#15 0x0000000000422fea in PyObject_Call (func=0x7f119e838c80, arg=<optimized out>, kw=<optimized out>) at Objects/abstract.c:2547
#16 0x00000000004272ed in instancemethod_call (func=0xdf0da0, arg=0x7f11a0a23810, kw=0x7f119e83b168)
    at Objects/classobject.c:2600
#17 0x0000000000422fea in PyObject_Call (func=func@entry=0x7f11a0ad6fa0, arg=arg@entry=0x7f11a0a23810,
    kw=kw@entry=0x7f119e83b168) at Objects/abstract.c:2547
#18 0x000000000048509d in slot_tp_init (self=<optimized out>, args=0x7f11a0a23810, kwds=0x7f119e83b168)
    at Objects/typeobject.c:5860
#19 0x000000000047f94a in type_call (type=<optimized out>, args=0x7f11a0a23810, kwds=0x7f119e83b168) at Objects/typeobject.c:765
#20 0x0000000000422fea in PyObject_Call (func=0xe836b0, arg=<optimized out>, kw=<optimized out>) at Objects/abstract.c:2547
#21 0x00000000004b3559 in ext_do_call (nk=<optimized out>, na=<optimized out>, flags=<optimized out>, pp_stack=<optimized out>,
    func=<optimized out>) at Python/ceval.c:4671
#22 PyEval_EvalFrameEx (f=0xdf0da0, throwflag=128) at Python/ceval.c:3033
#23 0x00000000004bb38b in PyEval_EvalCodeEx (co=0x7f11a0a17bb0, globals=0x80, locals=0x0, args=0x7f11a0b53da0, argcount=0,
    kws=0x501910 <posix_fork>, kwcount=0, defs=0x0, defcount=0, closure=0x0) at Python/ceval.c:3589
#24 0x00000000004b93eb in fast_function (nk=<optimized out>, na=<optimized out>, n=<optimized out>, pp_stack=<optimized out>,
    func=<optimized out>) at Python/ceval.c:4452
#25 call_function (oparg=<optimized out>, pp_stack=<optimized out>) at Python/ceval.c:4377
#26 PyEval_EvalFrameEx (f=0xdf0da0, throwflag=128) at Python/ceval.c:2994
#27 0x00000000004bb38b in PyEval_EvalCodeEx (co=0x7f11a0a6c930, globals=0x80, globals@entry=0x7f11a0aff168, locals=0x0,
    locals@entry=0x7f11a0aff168, args=0x0, argcount=argcount@entry=0, kws=0x501910 <posix_fork>, kws@entry=0x0, kwcount=0,
    defs=0x0, defcount=0, closure=0x0) at Python/ceval.c:3589
#28 0x00000000004bb4a9 in PyEval_EvalCode (co=co@entry=0x7f11a0a6c930, globals=globals@entry=0x7f11a0aff168,
    locals=locals@entry=0x7f11a0aff168) at Python/ceval.c:669
#29 0x00000000004e520f in run_mod (arena=0xe30b20, flags=0x7ffc93f34570, locals=0x7f11a0aff168, globals=0x7f11a0aff168,
    filename=0x548a06 "<string>", mod=0xe63020) at Python/pythonrun.c:1385
#30 PyRun_StringFlags (flags=0x7ffc93f34570, locals=0x7f11a0aff168, globals=0x7f11a0aff168, start=257,
    str=0xdc4010 "import test_fork_hanging; import subprocess; subprocess.check_output(\"ls\")\n") at Python/pythonrun.c:1348
#31 PyRun_SimpleStringFlags (
    command=command@entry=0xdc4010 "import test_fork_hanging; import subprocess; subprocess.check_output(\"ls\")\n",
    flags=flags@entry=0x7ffc93f34570) at Python/pythonrun.c:983
#32 0x0000000000415a80 in Py_Main (argc=3, argv=<optimized out>) at Modules/main.c:589
#33 0x00007f119fccab45 in __libc_start_main (main=0x4150c0 <main>, argc=3, argv=0x7ffc93f34728, init=<optimized out>,
    fini=<optimized out>, rtld_fini=<optimized out>, stack_end=0x7ffc93f34718) at libc-start.c:287
#34 0x00000000004150ee in _start ()
```

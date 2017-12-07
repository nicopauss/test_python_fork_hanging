#include <Python.h>
#include <pthread.h>

static void atfork_prepare(void)
{
    PyGILState_GetThisThreadState();
}

PyDoc_STRVAR(module_doc,
             "Simple module to test PyGILState_GetThisThreadState hanging "
             "with pthread_atfork() with patch 'Protect key list during "
             "fork()'");

PyMODINIT_FUNC inittest_fork_hanging(void)
{
    PyEval_InitThreads();
    Py_InitModule3("test_fork_hanging", NULL, module_doc);
    pthread_atfork(&atfork_prepare, NULL, NULL);
}

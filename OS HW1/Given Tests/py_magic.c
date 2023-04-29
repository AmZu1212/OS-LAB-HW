#include <Python.h>
#include <string.h>
#include "magic_api.h"


static PyObject *
posix_error(void)
{
  return PyErr_SetFromErrno(PyExc_OSError);
}


static PyObject *
py_get_wand(PyObject *self, PyObject *args)
{
  int power;
  char* secret;
  int status;

  if (!PyArg_ParseTuple(args, "is", &power, &secret))
      return NULL;
  status = magic_get_wand(power, secret);

  if (status < 0)
    return posix_error();

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_attack(PyObject *self, PyObject *args)
{
  int pid;
  int status;

  if (!PyArg_ParseTuple(args, "i", &pid))
    return NULL;

  status = magic_attack(pid);
  
  if (status < 0) {
    return posix_error();
  }

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_legilimens(PyObject *self, PyObject *args)
{
  int pid;
  int status;

  if (!PyArg_ParseTuple(args, "i", &pid))
    return NULL;

  status = magic_legilimens(pid);

  if (status < 0) {
    return posix_error();
  }

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_list_secrets(PyObject *self, PyObject *args)
{
  char (*secrets)[SECRET_MAXSIZE];
  size_t array_size, secret_size;
  size_t written, remaining;
  int i;
  PyObject* py_secrets;
  PyObject* ret;

  if (!PyArg_ParseTuple(args, "i", &array_size))
    return NULL;

  secrets = malloc(array_size * SECRET_MAXSIZE);
  if (!secrets) return PyErr_NoMemory();
  
  remaining = magic_list_secrets(secrets, array_size);

  if (remaining < 0) {
    free(secrets);
    return posix_error();
  }

  for (written=0; written<array_size; written++) {
      if (strnlen(secrets[written], SECRET_MAXSIZE) == 0) break;
  }

  py_secrets = PyTuple_New(written);
  if (!py_secrets) {
    free(secrets);
    return NULL;
  }

  for (i=0; i<written; i++) {
      secret_size = strnlen(secrets[i], SECRET_MAXSIZE);
      if (PyTuple_SetItem(py_secrets, i, Py_BuildValue("s#", secrets[i], secret_size)) != 0) {
          free(secrets);
          return NULL;
      }
  }
  free(secrets);

  ret = PyTuple_New(2);
  if (!ret) return NULL;
  if ((PyTuple_SetItem(ret, 0, py_secrets) != 0) ||
          (PyTuple_SetItem(ret, 1, Py_BuildValue("i", remaining)) != 0)) {
      return NULL;
  }

  return ret;
}

static PyMethodDef msgMethods[] = {
  {"get_wand",  py_get_wand, METH_VARARGS,
   "get_wand(power, secret)\n\nAttach a wand with a given power to the current process using the given secret.\n"},
  {"attack",  py_attack, METH_VARARGS,
   "attack(pid)\n\nAttack another process.\n"},
  {"legilimens",  py_legilimens, METH_VARARGS,
   "legilimens(pid)\n\nSteal the wand-secret from another process.\n"},
  {"list_secrets",  py_list_secrets, METH_VARARGS,
   "secrets, remaining = list_secrets(size)\n\nList the first SIZE stolen secrets and how many rememaining\n"},
  {NULL, NULL, 0, NULL} 
};


void
initpyMagic(void)
{
  (void) Py_InitModule("pyMagic", msgMethods);
}

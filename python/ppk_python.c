#include <ppasskeeper.h>

#include <Python.h>
#include <stdlib.h>

PyObject *wrap_getAvailableModulesCount(PyObject *o, PyObject * args)
{
	int num = getAvailableModulesCount();
	PyObject *result = Py_BuildValue("i", num);
	return result;
}

PyObject *wrap_getAvailableModules(PyObject *o, PyObject * args)
{
	struct PPassKeeper_Module modules[getAvailableModulesCount()];
	int num = getAvailableModules(modules, getAvailableModulesCount());
	PyObject *result = PyList_New(num);
	int i;
	for (i = 0; i < num; i++)
	{
		PyObject *module = PyTuple_New(2);
		PyTuple_SetItem(module, 0, Py_BuildValue("s", modules[i].id));
		PyTuple_SetItem(module, 1, Py_BuildValue("s", modules[i].display_name));
		PyList_SetItem(result, i, module);
	}
	return result;
}

PyObject *wrap_getNetworkPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username;
	int port;
	unsigned int flags=0;
	int ok = PyArg_ParseTuple(args, "ssisi", &module_id, &server, &port, &username, &flags);
	if (! ok) return 0;
	const char *result = getNetworkPassword(module_id, server, port, username, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setNetworkPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username, *pwd;
	int port;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssissi", &module_id, &server, &port, &username, &pwd, &flags);
	if (! ok) return 0;
	int result = setNetworkPassword(module_id, server, port, username, pwd, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "sssi", &module_id, &application_name, &username, &flags);
	if (! ok) return 0;
	const char *result = getApplicationPassword(module_id, application_name, username, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username, *pwd;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssssi", &module_id, &application_name, &username, &pwd, &flags);
	if (! ok) return 0;
	int result = setApplicationPassword(module_id, application_name, username, pwd, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssi", &module_id, &key, &flags);
	if (! ok) return 0;
	const char *result = getItem(module_id, key, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key, *item;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "sssi", &module_id, &key, &item, &flags);
	if (! ok) return 0;
	int result = setItem(module_id, key, item, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getLastError(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	const char *result = getLastError(module_id);
	return Py_BuildValue("s", result);
}

static PyMethodDef ppkMethods[] =
{
	{"getAvailableModulesCount", wrap_getAvailableModulesCount, METH_NOARGS, ""},
	{"getAvailableModules", wrap_getAvailableModules, METH_NOARGS, ""},
	{"getNetworkPassword", wrap_getNetworkPassword, METH_VARARGS, ""},
	{"setNetworkPassword", wrap_setNetworkPassword, METH_VARARGS, ""},
	{"getApplicationPassword", wrap_getApplicationPassword, METH_VARARGS, ""},
	{"setApplicationPassword", wrap_setApplicationPassword, METH_VARARGS, ""},
	{"getItem", wrap_getItem, METH_VARARGS, ""},
	{"setItem", wrap_setItem, METH_VARARGS, ""},
	{"getLastError", wrap_getLastError, METH_VARARGS, ""},
	{ NULL, NULL }
};

#if defined(WIN32) || defined(WIN64)
__declspec(dllexport)
#endif
void initppk(void)
{
	PyObject *m =
		Py_InitModule4(
				"ppk",
				ppkMethods,
				"",
				0,
				PYTHON_API_VERSION);
	return;
}

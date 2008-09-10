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
	int ok = PyArg_ParseTuple(args, "ssis", &module_id, &server, &port, &username);
	if (! ok) return 0;
	const char *result = getNetworkPassword(module_id, server, port, username);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setNetworkPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username, *pwd;
	int port;
	int ok = PyArg_ParseTuple(args, "ssiss", &module_id, &server, &port, &username, &pwd);
	if (! ok) return 0;
	int result = setNetworkPassword(module_id, server, port, username, pwd);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username;
	int ok = PyArg_ParseTuple(args, "sss", &module_id, &application_name, &username);
	if (! ok) return 0;
	const char *result = getApplicationPassword(module_id, application_name, username);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username, *pwd;
	int ok = PyArg_ParseTuple(args, "ssss", &module_id, &application_name, &username, &pwd);
	if (! ok) return 0;
	int result = setApplicationPassword(module_id, application_name, username, pwd);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key;
	int ok = PyArg_ParseTuple(args, "ss", &module_id, &key);
	if (! ok) return 0;
	const char *result = getItem(module_id, key);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key, *item;
	int ok = PyArg_ParseTuple(args, "sss", &module_id, &key, &item);
	if (! ok) return 0;
	int result = setItem(module_id, key, item);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getNetworkPassword_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username;
	int port;
	int ok = PyArg_ParseTuple(args, "ssis", &module_id, &server, &port, &username);
	if (! ok) return 0;
	const char *result = getNetworkPassword_silent(module_id, server, port, username);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setNetworkPassword_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username, *pwd;
	int port;
	int ok = PyArg_ParseTuple(args, "ssiss", &module_id, &server, &port, &username, &pwd);
	if (! ok) return 0;
	int result = setNetworkPassword_silent(module_id, server, port, username, pwd);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getApplicationPassword_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username;
	int ok = PyArg_ParseTuple(args, "sss", &module_id, &application_name, &username);
	if (! ok) return 0;
	const char *result = getApplicationPassword_silent(module_id, application_name, username);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setApplicationPassword_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username, *pwd;
	int ok = PyArg_ParseTuple(args, "ssss", &module_id, &application_name, &username, &pwd);
	if (! ok) return 0;
	int result = setApplicationPassword_silent(module_id, application_name, username, pwd);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getItem_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *key;
	int ok = PyArg_ParseTuple(args, "ss", &module_id, &key);
	if (! ok) return 0;
	const char *result = getItem_silent(module_id, key);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setItem_silent(PyObject *o, PyObject * args)
{
	const char *module_id, *key, *item;
	int ok = PyArg_ParseTuple(args, "sss", &module_id, &key, &item);
	if (! ok) return 0;
	int result = setItem_silent(module_id, key, item);
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
	{"getNetworkPassword_silent", wrap_getNetworkPassword_silent, METH_VARARGS, ""},
	{"setNetworkPassword_silent", wrap_setNetworkPassword_silent, METH_VARARGS, ""},
	{"getApplicationPassword_silent", wrap_getApplicationPassword_silent, METH_VARARGS, ""},
	{"setApplicationPassword_silent", wrap_setApplicationPassword_silent, METH_VARARGS, ""},
	{"getItem_silent", wrap_getItem_silent, METH_VARARGS, ""},
	{"setItem_silent", wrap_setItem_silent, METH_VARARGS, ""},
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

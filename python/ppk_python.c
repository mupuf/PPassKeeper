#include <ppasskeeper.h>

#include <Python.h>
#include <stdlib.h>

PyObject *wrap_getAvailableModulesCount(PyObject *o, PyObject * args)
{
	int num = ppk_getAvailableModulesCount();
	PyObject *result = Py_BuildValue("i", num);
	return result;
}

PyObject *wrap_getAvailableModules(PyObject *o, PyObject * args)
{
	unsigned int modules_count=ppk_getAvailableModulesCount();
	struct PPassKeeper_Module modules[modules_count];				//May cause portabilty issues between compilers
	int num = ppk_getAvailableModules(modules, modules_count);
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

PyObject *wrap_moduleAvailable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	enum ppk_boolean available = ppk_moduleAvailable(module_id);
	PyObject *result = Py_BuildValue("b", available);
	return result;
}

PyObject *wrap_getNetworkPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username;
	int port;
	unsigned int flags=0;
	int ok = PyArg_ParseTuple(args, "ssisI", &module_id, &server, &port, &username, &flags);
	if (! ok) return 0;
	const char *result = ppk_getNetworkPassword(module_id, server, port, username, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setNetworkPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username, *pwd;
	int port;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssissI", &module_id, &server, &port, &username, &pwd, &flags);
	if (! ok) return 0;
	int result = ppk_setNetworkPassword(module_id, server, port, username, pwd, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "sssI", &module_id, &application_name, &username, &flags);
	if (! ok) return 0;
	const char *result = ppk_getApplicationPassword(module_id, application_name, username, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setApplicationPassword(PyObject *o, PyObject * args)
{
	const char *module_id, *application_name, *username, *pwd;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssssI", &module_id, &application_name, &username, &pwd, &flags);
	if (! ok) return 0;
	int result = ppk_setApplicationPassword(module_id, application_name, username, pwd, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssI", &module_id, &key, &flags);
	if (! ok) return 0;
	const char *result = ppk_getItem(module_id, key, flags);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setItem(PyObject *o, PyObject * args)
{
	const char *module_id, *key, *item;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "sssI", &module_id, &key, &item, &flags);
	if (! ok) return 0;
	int result = ppk_setItem(module_id, key, item, flags);
	return Py_BuildValue("i", result);
}

PyObject *wrap_isWritable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	enum ppk_boolean available = ppk_isWritable(module_id);
	PyObject *result = Py_BuildValue("b", available);
	return result;
}

PyObject *wrap_securityLevel(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	enum ppk_security_level result = ppk_securityLevel(module_id);
	return Py_BuildValue("i", result);
}

PyObject *wrap_getPasswordListCount(PyObject *o, PyObject * args)
{
	const char *module_id;
	int type, flags;
	int ok = PyArg_ParseTuple(args, "sii", &module_id, &type, &flags);
	if (! ok) return 0;
	int num = ppk_getPasswordListCount(module_id, type, flags);
	PyObject *result = Py_BuildValue("i", num);
	return result;
}

PyObject *wrap_getPasswordList(PyObject *o, PyObject * args)
{
	const char *module_id;
	int type, flags;

	int ok = PyArg_ParseTuple(args, "siI", &module_id, &type, &flags);
	if (! ok) return 0;

	unsigned int pwd_count = ppk_getPasswordListCount(module_id, type, flags);
	PyObject *result = PyList_New(pwd_count);

	if(type==ppk_network)
	{
		struct PPassKeeper_module_entry_net pwds[pwd_count];							//May cause portabilty issues between compilers
		int num = ppk_getPasswordList(module_id, type, pwds, pwd_count, flags);
	
		int i;
		for (i = 0; i < num && i < pwd_count; i++)
		{
			PyObject *module = PyTuple_New(3);
			PyTuple_SetItem(module, 0, Py_BuildValue("s", pwds[i].host));
			PyTuple_SetItem(module, 1, Py_BuildValue("s", pwds[i].login));
			PyTuple_SetItem(module, 2, Py_BuildValue("i", pwds[i].port));
			PyList_SetItem(result, i, module);
		}
	}
	else if(type==ppk_application)
	{
		struct PPassKeeper_module_entry_app pwds[pwd_count];							//May cause portabilty issues between compilers
		int num = ppk_getPasswordList(module_id, type, pwds, pwd_count, flags);
	
		int i;
		for (i = 0; i < num && i < pwd_count; i++)
		{
			PyObject *module = PyTuple_New(2);
			PyTuple_SetItem(module, 0, Py_BuildValue("s", pwds[i].app_name));
			PyTuple_SetItem(module, 1, Py_BuildValue("s", pwds[i].username));
			PyList_SetItem(result, i, module);
		}
	}
	else if(type==ppk_item)
	{
		struct PPassKeeper_module_entry_item pwds[pwd_count];							//May cause portabilty issues between compilers
		int num = ppk_getPasswordList(module_id, type, pwds, pwd_count, flags);
	
		int i;
		for (i = 0; i < num && i < pwd_count; i++)
		{
			PyObject *module = PyTuple_New(1);
			PyTuple_SetItem(module, 0, Py_BuildValue("s", pwds[i].key));
			PyList_SetItem(result, i, module);
		}
	}
	else
		return Py_BuildValue("");


	return result;
}

PyObject *wrap_getLastError(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	const char *result = ppk_getLastError(module_id);
	return Py_BuildValue("s", result);
}

PyObject *wrap_setCustomPromptMessage(PyObject *o, PyObject * args)
{
	const char *module_id, *customMessage;
	int ok = PyArg_ParseTuple(args, "ss", &module_id, &customMessage);
	if (! ok) return 0;
	enum ppk_boolean res = ppk_setCustomPromptMessage(module_id, customMessage);
	PyObject *result = Py_BuildValue("b", res);
	return result;
}

static PyMethodDef ppkMethods[] =
{
	{"getAvailableModulesCount", wrap_getAvailableModulesCount, METH_NOARGS, ""},
	{"getAvailableModules", wrap_getAvailableModules, METH_NOARGS, ""},
	{"moduleAvailable", wrap_moduleAvailable, METH_VARARGS, ""},
	{"getNetworkPassword", wrap_getNetworkPassword, METH_VARARGS, ""},
	{"setNetworkPassword", wrap_setNetworkPassword, METH_VARARGS, ""},
	{"getApplicationPassword", wrap_getApplicationPassword, METH_VARARGS, ""},
	{"setApplicationPassword", wrap_setApplicationPassword, METH_VARARGS, ""},
	{"getItem", wrap_getItem, METH_VARARGS, ""},
	{"setItem", wrap_setItem, METH_VARARGS, ""},
	{"isWritable", wrap_isWritable, METH_VARARGS, ""},
	{"securityLevel", wrap_securityLevel, METH_VARARGS, ""},
	{"getPasswordListCount", wrap_getPasswordListCount, METH_VARARGS, ""},
	{"getPasswordList", wrap_getPasswordList, METH_VARARGS, ""},
	{"getLastError", wrap_getLastError, METH_VARARGS, ""},
	{"setCustomPromptMessage", wrap_setCustomPromptMessage, METH_VARARGS, ""},
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

	PyModule_AddIntConstant(m, "sec_lowest", ppk_sec_lowest);
	PyModule_AddIntConstant(m, "sec_scrambled", ppk_sec_scrambled);
	PyModule_AddIntConstant(m, "sec_safe", ppk_sec_safe);
	PyModule_AddIntConstant(m, "sec_perfect", ppk_sec_perfect);

	PyModule_AddIntConstant(m, "network", ppk_network);
	PyModule_AddIntConstant(m, "application", ppk_application);
	PyModule_AddIntConstant(m, "item", ppk_item);

	PyModule_AddIntConstant(m, "rf_none", ppk_rf_none);
	PyModule_AddIntConstant(m, "rf_silent", ppk_rf_silent);
	PyModule_AddIntConstant(m, "wf_none", ppk_wf_none);
	PyModule_AddIntConstant(m, "wf_silent", ppk_wf_silent);
	PyModule_AddIntConstant(m, "lf_none", ppk_lf_none);
	PyModule_AddIntConstant(m, "lf_silent", ppk_lf_silent);

	return;
}

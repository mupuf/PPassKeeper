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
	ppk_module modules[modules_count];				//May cause portabilty issues between compilers
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
	ppk_boolean available = ppk_moduleAvailable(module_id);
	PyObject *result = Py_BuildValue("b", available);
	return result;
}

PyObject *wrap_getEntry(PyObject *o, PyObject * args, PyObject * kwargs)
{
	const char *module_id;
	ppk_entry entry;
	unsigned int type;
	const char *host = 0, *login = 0, *app_name = 0, *username = 0, *item = 0;
	unsigned int port = 0;
	unsigned int flags = 0;
	static char *kwlist[] = { "module_id", "type", "host", "login", "port", "app_name", "username", "item", "flags", NULL };
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "si|sshsssi", kwlist, &module_id,
			&type, &host, &login, &port, &app_name, &username, &item, &flags);
	switch (type)
	{
	case ppk_network:
		ok = ok && host && login && port && ! app_name && ! username && ! item;
		break;
	case ppk_application:
		ok = ok && ! host && ! login && ! port && app_name && username && ! item;
		break;
	case ppk_item:
		ok = ok && ! host && ! login && ! port && ! app_name && ! username && item;
		break;
	default:
		ok = 0;
	};

	if (! ok)
	{
	    PyErr_BadArgument();
	    return 0;
	}
	switch (type)
	{
	case ppk_network:
		entry.type = ppk_network;
		entry.net.host = host;
		entry.net.login = login;
		entry.net.port = port;
		break;
	case ppk_application:
		entry.type = ppk_application;
		entry.app.app_name = app_name;
		entry.app.username = username;
		break;
	case ppk_item:
		entry.type = ppk_item;
		entry.item = item;
	};
	ppk_data data;

	if (! ppk_getEntry(module_id, entry, &data, flags))
		return Py_None;
	
	PyObject *result = PyDict_New();
	PyDict_SetItemString(result, "type", Py_BuildValue("i", data.type));
	if (data.type == ppk_string)
		PyDict_SetItemString(result, "string", Py_BuildValue("s", data.string));
	else if (data.type == ppk_blob)
		//Untested
		PyDict_SetItemString(result, "blob", Py_BuildValue("t#", data.blob.data, data.blob.size));

	return result;
}

PyObject *wrap_setEntry(PyObject *o, PyObject * args)
{
	const char *module_id;
	ppk_entry entry;
	unsigned int type;
	const char *host = 0, *login = 0, *app_name = 0, *username = 0, *item = 0;
	unsigned int port = 0;
	unsigned int flags = 0;
	static char *kwlist[] = { "module_id", "type", "host", "login", "port", "app_name", "username", "item", "flags", NULL };
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "si|sshsssi", kwlist, &module_id,
			&type, &host, &login, &port, &app_name, &username, &item, &flags);
}

/*
PyObject *wrap_removeEntry(PyObject *o, PyObject * args)
{
	const char *module_id, *server, *username, *pwd;
	int port;
	unsigned int flags;
	int ok = PyArg_ParseTuple(args, "ssissI", &module_id, &server, &port, &username, &pwd, &flags);
	if (! ok) return 0;
	int result = ppk_setNetworkPassword(module_id, server, port, username, pwd, flags);
	return Py_BuildValue("i", result);
}
*/

PyObject *wrap_isWritable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	ppk_boolean available = ppk_isWritable(module_id);
	PyObject *result = Py_BuildValue("b", available);
	return result;
}

PyObject *wrap_securityLevel(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	ppk_security_level result = ppk_securityLevel(module_id);
	return Py_BuildValue("i", result);
}

/*
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
*/

/*
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
*/

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
	ppk_boolean res = ppk_setCustomPromptMessage(module_id, customMessage);
	PyObject *result = Py_BuildValue("b", res);
	return result;
}

static PyMethodDef ppkMethods[] =
{
	{"getAvailableModulesCount", wrap_getAvailableModulesCount, METH_NOARGS, ""},
	{"getAvailableModules", wrap_getAvailableModules, METH_NOARGS, ""},
	{"moduleAvailable", wrap_moduleAvailable, METH_VARARGS, ""},
	{"getEntry", (PyCFunction) wrap_getEntry, METH_VARARGS | METH_KEYWORDS, ""},
//	{"setEntry", wrap_setEntry, METH_KEYWORDS, ""},
//	{"removeEntry", wrap_removeEntry, METH_KEYWORDS, ""},
	{"isWritable", wrap_isWritable, METH_VARARGS, ""},
	{"securityLevel", wrap_securityLevel, METH_VARARGS, ""},
//	{"getPasswordListCount", wrap_getPasswordListCount, METH_VARARGS, ""},
//	{"getPasswordList", wrap_getPasswordList, METH_VARARGS, ""},
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

	PyModule_AddIntConstant(m, "string", ppk_string);
	PyModule_AddIntConstant(m, "blob", ppk_blob);

	PyModule_AddIntConstant(m, "rf_none", ppk_rf_none);
	PyModule_AddIntConstant(m, "rf_silent", ppk_rf_silent);
	PyModule_AddIntConstant(m, "wf_none", ppk_wf_none);
	PyModule_AddIntConstant(m, "wf_silent", ppk_wf_silent);
	PyModule_AddIntConstant(m, "lf_none", ppk_lf_none);
	PyModule_AddIntConstant(m, "lf_silent", ppk_lf_silent);

	return;
}

#include <ppasskeeper.h>

#include <Python.h>
#include <stdlib.h>

PyObject *wrap_getAvailableModulesCount(PyObject *o, PyObject * args)
{
	int num = ppk_getAvailableModulesCount();
	PyObject *result = Py_BuildValue("I", num);
	return result;
}

PyObject *wrap_isLocked(PyObject *o, PyObject * args)
{
	return ppk_isLocked()?Py_True:Py_False;
}

PyObject *wrap_setPassword(PyObject *o, PyObject * args)
{
	const char *pwd;
	int ok = PyArg_ParseTuple(args, "s", &pwd);
	if (! ok) return 0;
	
	return ppk_setPassword(pwd)?Py_True:Py_False;
}

PyObject *wrap_unlock(PyObject *o, PyObject * args)
{
	const char *pwd;
	int ok = PyArg_ParseTuple(args, "s", &pwd);
	if (! ok) return 0;
	
	return ppk_unlock(pwd)?Py_True:Py_False;
}
	
PyObject *wrap_saveParam(PyObject *o, PyObject * args)
{
	const char *module_id, *key, *value;
	int ok = PyArg_ParseTuple(args, "sss", &module_id, &key, &value);
	if (! ok) return 0;
	
	ppk_boolean res = ppk_saveParam(module_id, key, value);
	
	return res==PPK_TRUE?Py_True:Py_False;
}
	
///																	<--------- Doesn't seem to be working !
PyObject *wrap_getParam(PyObject *o, PyObject * args)
{
	const char *module_id, *key;
	int ok = PyArg_ParseTuple(args, "ss", &module_id, &key);
	if (! ok) return 0;
	
	char returned[501];
	ppk_boolean res = ppk_getParam(module_id, key, returned, sizeof(returned));
	
	if(res==PPK_FALSE)
		returned[0]='\0';

	return Py_BuildValue("s", returned);
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

static void buildEntry(ppk_entry *e, unsigned int type, const char *host, const char *login, unsigned int port, const char *app_name, const char *username, const char *item)
{
	switch (type)
	{
	case ppk_network:
		e->type = ppk_network;
		e->net.host = host;
		e->net.login = login;
		e->net.port = port;
		break;
	case ppk_application:
		e->type = ppk_application;
		e->app.app_name = app_name;
		e->app.username = username;
		break;
	case ppk_item:
		e->type = ppk_item;
		e->item = item;
	};
}

static void buildData(ppk_data *d, unsigned int data_type, const char *string, const void *blob, unsigned long size)
{
	switch (data_type)
	{
	case ppk_string:
		d->type = ppk_string;
		d->string = string;
		break;
	case ppk_blob:
		d->type = ppk_blob;
		d->blob.data = blob;
		d->blob.size = size;
	}
}

static PyObject *buildEntryList(const char *module_id, unsigned int entry_types, unsigned int flags)
{
	unsigned int nb = ppk_getEntryListCount(module_id, entry_types, flags);
	ppk_entry entryList[nb];
	nb = ppk_getEntryList(module_id, entry_types, entryList, nb, flags);

	PyObject *result = PyList_New(nb), *dict;
	int i;
	for (i = 0; i < nb; ++i)
	{
		dict = PyDict_New();
		PyDict_SetItemString(dict, "type", Py_BuildValue("I", entryList[i].type));
		switch (entryList[i].type)
		{
		case ppk_network:
			PyDict_SetItemString(dict, "host", Py_BuildValue("s", entryList[i].net.host));
			PyDict_SetItemString(dict, "login", Py_BuildValue("s", entryList[i].net.login));
			PyDict_SetItemString(dict, "port", Py_BuildValue("H", entryList[i].net.port));
			break;
		case ppk_application:
			PyDict_SetItemString(dict, "app_name", Py_BuildValue("s", entryList[i].app.app_name));
			PyDict_SetItemString(dict, "username", Py_BuildValue("s", entryList[i].app.username));
			break;
		case ppk_item:
			PyDict_SetItemString(dict, "item", Py_BuildValue("s", entryList[i].item));
			break;
		}
		PyList_SetItem(result, i, dict);
	}

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
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "sI|ssHsssI", kwlist, &module_id,
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

	buildEntry(&entry, type, host, login, port, app_name, username, item);

	ppk_data data;

	if (! ppk_getEntry(module_id, entry, &data, flags))
		return Py_None;
	
	PyObject *result = PyDict_New();
	PyDict_SetItemString(result, "type", Py_BuildValue("I", data.type));
	if (data.type == ppk_string)
		PyDict_SetItemString(result, "string", Py_BuildValue("s", data.string));
	else if (data.type == ppk_blob)
		//Untested
		PyDict_SetItemString(result, "blob", Py_BuildValue("t#", data.blob.data, data.blob.size));

	return result;
}

PyObject *wrap_setEntry(PyObject *o, PyObject * args, PyObject * kwargs)
{
	const char *module_id;
	ppk_entry entry;
	ppk_data data;
	unsigned int type, data_type;
	const char *host = 0, *login = 0, *app_name = 0, *username = 0, *item = 0, *string = 0, *blob = 0;
	int size = 0;
	unsigned int port = 0;
	unsigned int flags = 0;
	static char *kwlist[] = { "module_id", "type", "data_type", "host", "login", "port", "app_name", "username", "item", "string", "blob", "flags", NULL };
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "sII|ssHsssst#I", kwlist, &module_id,
			&type, &data_type, &host, &login, &port, &app_name, &username, &item, &string, &blob, &size, &flags);

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

	switch (data_type)
	{
	case ppk_string:
		ok = ok && string && ! blob;
		break;
	case ppk_blob:
		ok = ok && blob && size && ! string;
		break;
	default:
		ok = 0;
	}

	if (! ok)
	{
	    PyErr_BadArgument();
	    return 0;
	}

	buildEntry(&entry, type, host, login, port, app_name, username, item);
	buildData(&data, data_type, string, blob, size);

	return (ppk_setEntry(module_id, entry, data, flags)) ? Py_True : Py_False;
}

PyObject *wrap_removeEntry(PyObject *o, PyObject * args, PyObject * kwargs)
{
	const char *module_id;
	ppk_entry entry;
	unsigned int type;
	const char *host = 0, *login = 0, *app_name = 0, *username = 0, *item = 0;
	unsigned int port = 0;
	unsigned int flags = 0;
	static char *kwlist[] = { "module_id", "type", "host", "login", "port", "app_name", "username", "item","flags", NULL };
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "sI|ssHsssI", kwlist, &module_id, &type, &host, &login, &port, &app_name, &username, &item, &flags);

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

	buildEntry(&entry, type, host, login, port, app_name, username, item);

	return (ppk_removeEntry(module_id, entry, flags)) ? Py_True : Py_False;
}

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
	PyObject *obj=Py_BuildValue("I", result);
	return obj;
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
	ppk_boolean res = ppk_setCustomPromptMessage(module_id, customMessage);
	PyObject *result = Py_BuildValue("b", res);
	return result;
}

PyObject *wrap_readFlagsAvailable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	return Py_BuildValue("I", ppk_readFlagsAvailable(module_id));
}

PyObject *wrap_writeFlagsAvailable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	return Py_BuildValue("I", ppk_writeFlagsAvailable(module_id));
}

PyObject *wrap_listingFlagsAvailable(PyObject *o, PyObject * args)
{
	const char *module_id;
	int ok = PyArg_ParseTuple(args, "s", &module_id);
	if (! ok) return 0;
	return Py_BuildValue("I", ppk_listingFlagsAvailable(module_id));
}

PyObject *wrap_getEntryListCount(PyObject *o, PyObject * args)
{
	const char *module_id;
	unsigned int entry_types, flags;
	int ok = PyArg_ParseTuple(args, "sII", &module_id, &entry_types, &flags);
	if (! ok) return 0;
	return Py_BuildValue("I", ppk_getEntryListCount(module_id, entry_types, flags));
}

PyObject *wrap_getEntryList(PyObject *o, PyObject * args)
{
	const char *module_id;
	unsigned int entry_types, flags;
	int ok = PyArg_ParseTuple(args, "sII", &module_id, &entry_types, &flags);
	if (! ok) return 0;
	return buildEntryList(module_id, entry_types, flags);
}

PyObject *wrap_entryExists(PyObject *o, PyObject * args, PyObject * kwargs)
{
	const char *module_id;
	ppk_entry entry;
	unsigned int type;
	const char *host = 0, *login = 0, *app_name = 0, *username = 0, *item = 0;
	unsigned int port = 0;
	unsigned int flags = 0;
	static char *kwlist[] = { "module_id", "type", "host", "login", "port", "app_name", "username", "item","flags", NULL };
	int ok = PyArg_ParseTupleAndKeywords(args, kwargs, "sI|ssHsssI", kwlist, &module_id, &type, &host, &login, &port, &app_name, &username, &item, &flags);

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

	buildEntry(&entry, type, host, login, port, app_name, username, item);

	return (ppk_entryExists(module_id, entry, flags)) ? Py_True : Py_False;
}

PyObject *wrap_maxDataSize(PyObject *o, PyObject * args)
{
	const char *module_id;
	unsigned int data_type;
	int ok = PyArg_ParseTuple(args, "sI", &module_id, &data_type);
	if (! ok) return 0;
	return Py_BuildValue("I", ppk_maxDataSize(module_id, data_type));
}

static PyMethodDef ppkMethods[] =
{
	{"getAvailableModulesCount", wrap_getAvailableModulesCount, METH_NOARGS, ""},
	{"getAvailableModules", wrap_getAvailableModules, METH_NOARGS, ""},
	{"moduleAvailable", wrap_moduleAvailable, METH_VARARGS, ""},
	{"readFlagsAvailable", wrap_readFlagsAvailable, METH_VARARGS, ""},
	{"writeFlagsAvailable", wrap_writeFlagsAvailable, METH_VARARGS, ""},
	{"listingFlagsAvailable", wrap_listingFlagsAvailable, METH_VARARGS, ""},
	{"getEntry", (PyCFunction) wrap_getEntry, METH_VARARGS | METH_KEYWORDS, ""},
	{"setEntry", (PyCFunction) wrap_setEntry, METH_VARARGS | METH_KEYWORDS, ""},
	{"removeEntry", (PyCFunction) wrap_removeEntry, METH_VARARGS | METH_KEYWORDS, ""},
	{"getEntryListCount", wrap_getEntryListCount, METH_VARARGS, ""},
	{"getEntryList", wrap_getEntryList, METH_VARARGS, ""},
	{"entryExists", (PyCFunction) wrap_entryExists, METH_VARARGS | METH_KEYWORDS, ""},
	{"maxDataSize", wrap_maxDataSize, METH_VARARGS, ""},
	{"isWritable", wrap_isWritable, METH_VARARGS, ""},
	{"securityLevel", wrap_securityLevel, METH_VARARGS, ""},
	{"getLastError", wrap_getLastError, METH_VARARGS, ""},
	{"setCustomPromptMessage", wrap_setCustomPromptMessage, METH_VARARGS, ""},
	{"isLocked", wrap_isLocked, METH_VARARGS, ""},
	{"setPassword", wrap_setPassword, METH_VARARGS, ""},
	{"unlock", wrap_unlock, METH_VARARGS, ""},
	{"saveParam", wrap_saveParam, METH_VARARGS, ""},
	{"getParam", wrap_getParam, METH_VARARGS, ""},
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

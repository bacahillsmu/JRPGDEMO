#pragma once
#include "ThirdParty/Python/Python.h"


// Define the function you want
PyObject* Print(PyObject* self, PyObject* args)
{
	self;

	int arg_count = (int)PyTuple_Size(args);
	if (arg_count > 0) {
		PyObject* arg0 = PyTuple_GetItem(args, 0);

		PyObject* str = nullptr;
		if (PyUnicode_CheckExact(arg0)) {
			str = PyUnicode_AsEncodedString(arg0, "utf-8", "~E~");
		}
		else {
			PyObject* repr = PyObject_Repr(arg0);
			str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
			Py_DecRef(repr);
		}

		char const* bytes = PyBytes_AS_STRING(str);
		g_theDevConsole->Print(bytes);

		Py_DecRef(str);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

// Describe it to the python
static PyMethodDef MyPyMethods[] = {

	// python func name,		C Func to call,				Type,					Help Message
	{"Print",					Print,						METH_VARARGS,			"Logs using the system log"},
	{nullptr,					nullptr,					0,						nullptr} // terminator element

};

// define the module
static PyModuleDef MyModule = {

	// base,					name,			documentation,	size,		methods,			slots,			tarverse_cb,	clear_cb,		free_cb
	PyModuleDef_HEAD_INIT,		"bEngine",		nullptr,		-1,			MyPyMethods,		nullptr,		nullptr,		nullptr,		nullptr

};

/*
import bEngine
bEngine.Print("Hello, World");
*/

static PyObject* MyModuleInit()
{
	return PyModule_Create(&MyModule);
}
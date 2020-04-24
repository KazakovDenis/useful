/*
Создание Python-библиотеки на Си
https://realpython.com/build-python-c-extension-module/
*/

#include <Python.h>


// Создаём собственное исключение
static PyObject *StringTooShortError = NULL;


// Создаём обёртку над С-функцией для Python
static PyObject *method_fputs(PyObject *self, PyObject *args) {
    char *str, *filename = NULL;
    int bytes_copied = -1;

    // Получаем значения аргументов
    if(!PyArg_ParseTuple(args, "ss", &str, &filename)) {
       return NULL;
    }

    // Вызываем исключение
    if (strlen(str) < 10) {
        PyErr_SetString(StringTooShortError, "String length must be greater than 10");
        return NULL;
    }

    FILE *fp = fopen(filename, "w");
    bytes_copied = fputs(str, fp);
    fclose(fp);

    return PyLong_FromLong(bytes_copied);
}


// Создаём обёртку над С-классом для Python
// Создаём структуру, хранящую данные
typedef struct {
    PyObject_HEAD
    int ival;
} CustomObject;

// Создаем структуру описывающую тип:
static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fputs.Custom",
    .tp_doc = "Custom objects",
    .tp_basicsize = sizeof(CustomObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};


// Из 2 нижеследующих структур данные будут генерироваться в ру-файл
// Описываем входящие в модуль функции
static PyMethodDef FputsMethods[] = {
    {"fputs", method_fputs, METH_VARARGS, "PyMethodDef: fputs function docstring"},
    {NULL, NULL, 0, NULL}
};


// Задаём метаданные для модуля
static struct PyModuleDef fputsmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "c_ext",
    .m_doc = "PyModuleDef: module docstring",
    .m_size = -1,
    FputsMethods
};


// Инициализируем модуль
PyMODINIT_FUNC
PyInit_c_ext(void) {

    PyObject *module = PyModule_Create(&fputsmodule);
    if (module == NULL)
        return NULL;

    // Добавляем класс в модуль:
    if (PyType_Ready(&CustomType) < 0)
        return NULL;

    Py_INCREF(&CustomType);
    if (PyModule_AddObject(module, "Custom", (PyObject *) &CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(module);
        return NULL;
    }

    // Инициализируем объект исключения
    StringTooShortError = PyErr_NewException("fputs.StringTooShortError", NULL, NULL);

    // Добавляем исключение в модуль
    PyModule_AddObject(module, "StringTooShortError", StringTooShortError);
    
    // Добавляем константы
    PyModule_AddIntConstant(module, "FPUTS_FLAG", 64);

    // А также через макрос
    #define FPUTS_MACRO 256
    PyModule_AddIntMacro(module, FPUTS_MACRO);

    return module;
}

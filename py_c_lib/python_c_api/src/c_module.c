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


// Из 2 нижеследующих структур данные будут генерироваться в ру-файл
// Описываем входящие в модуль функции
static PyMethodDef FputsMethods[] = {
    {"fputs", method_fputs, METH_VARARGS, "PyMethodDef: fputs function docstring"},
    {NULL, NULL, 0, NULL}
};


// Задаём метаданные для модуля
static struct PyModuleDef fputsmodule = {
    PyModuleDef_HEAD_INIT,
    "c_ext",
    "PyModuleDef: module docstring",
    -1,
    FputsMethods
};


// Инициализируем модуль
PyMODINIT_FUNC PyInit_c_ext(void) {
    PyObject *module = PyModule_Create(&fputsmodule);

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

/*
Создание Python-библиотеки на Си
*/
#include "class.c"
#include "functions.c"

#ifndef Py_PYTHON_H
#define PY_SSIZE_T_CLEAN     // рекомендуется определять этот макрос
#include <Python.h>          // включать этот заголовок перед другими стандартными
#endif


// Описываем входящие в модуль функции
static PyMethodDef MyLibMethods[] = {
    {
        "fputs",                             // название функции
        method_fputs,                        // имя функции на Си
        METH_VARARGS,                        // способ передачи аргументов: METH_VARARGS (переменное кол-во) / METH_NOARGS (нет) 
        "Функция для записи текста в файл"   // докстринг
    },
    {NULL, NULL, 0, NULL}                    // метка окончания спиcка аргументов
};


// Задаём метаданные для модуля
static struct PyModuleDef mymodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "my_ext",
    .m_doc = "Реализация Python-интерфейса для С-кода",
    .m_size = -1,
    MyLibMethods
};


// Инициализируем модуль PyInit_[название_модуля_из_setup.py]
PyMODINIT_FUNC 
PyInit_my_ext(void) {

    PyObject *module;

    // Инициализируем наш класс, заполняя его поля дефолтными значениями
    if (PyType_Ready(&MyClassType) < 0)
        return NULL;                          // если не хватило памяти

    module = PyModule_Create(&mymodule);
    if (module == NULL)
        return NULL;

    Py_INCREF(&MyClassType);                  // увеличивает счётчик ссылок
    if (PyModule_AddObject(module, "MyClass", (PyObject *) &MyClassType) < 0) {
        Py_DECREF(&MyClassType);
        Py_DECREF(module);
        return NULL;
    }

    // Инициализируем объект исключения
    StringTooShortError = PyErr_NewException("mylib.StringTooShortError", NULL, NULL);

    // Добавляем объекты в модуль
    PyModule_AddObject(module, "StringTooShortError", StringTooShortError);
    PyModule_AddIntConstant(module, "MYLIB_FLAG", 64);                             // Добавляем константы
    #define MYLIB_MACRO 256                                                        // А также через макрос
    PyModule_AddIntMacro(module, MYLIB_MACRO);

    return module;
}

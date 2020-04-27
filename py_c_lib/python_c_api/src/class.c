#ifndef Test_CLASS_H

#define PY_SSIZE_T_CLEAN     // рекомендуется
#include <Python.h>          // включать перед другими заголовками
#include "structmember.h"


// Создаём структуру c полями создаваемого класса
typedef struct {
    PyObject_HEAD        // обязательный макрос, ниже определяем поля
    PyObject *first;
    PyObject *last;
    int number;
} MyClassObject;


// функция-деструктор для освобождения памяти из-под объекта
static void
MyClass_dealloc(MyClassObject *self)
{
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
    Py_TYPE(self)->tp_free((PyObject *) self);
}


// реализуем метод __new__
static PyObject *
MyClass_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    MyClassObject *self;
    self = (MyClassObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->first = PyUnicode_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->last = PyUnicode_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->number = 0;
    }
    return (PyObject *) self;
}


// реализуем метод __init__
static int
MyClass_init(MyClassObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"first", "last", "number", NULL};
    PyObject *first = NULL, *last = NULL, *tmp;

    // разбор строки аргументов
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
                                     &first,
                                     &last,
                                     &self->number))
        return -1;

    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }
    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }
    return 0;
}


// описываем поля:
static PyMemberDef MyClass_members[] = {
    {
        "first",                          // название поля
        T_OBJECT_EX,                      // тип поля
        offsetof(MyClassObject, first),   // смещение элемента в структуре
        0,                                // флаг (READONLY, READ_RESTRICTED и т.д.)
        "first name"                      // описание
    },
    {"last", T_OBJECT_EX, offsetof(MyClassObject, last), 0, "last name"},
    {"number", T_INT, offsetof(MyClassObject, number), 0, "custom number"},
    {NULL}                                // метка окончания спиcка полей
};


// реализуем метод .name()
static PyObject *
MyClass_name(MyClassObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }
    if (self->last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S", self->first, self->last);
}


// описываем методы класса:
static PyMethodDef MyClass_methods[] = {
    {"name",                                // название 
    (PyCFunction) MyClass_name,             // имя метода на Си с приведением к PyCFunction
    METH_NOARGS,                            // способ передачи аргументов: METH_VARARGS (переменное кол-во) / METH_NOARGS (нет) 
    "Return the name, combining \           
    the first and last name"                // описание
    },
    {NULL}                                  // метка окончания спиcка аргументов
};


// Создаем структуру, описывающую класс:
static PyTypeObject MyClassType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "mylib.MyClass",                              // имя вида модуль.Класс
    .tp_doc = "MyClass objects",                             // докстринг класса
    .tp_basicsize = sizeof(MyClassObject),
    .tp_itemsize = 0,                                        // размер экземпляра, если отличается от tp_basicsize
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,    // различные флаги доступа
    .tp_new = MyClass_new,                                   // реализация __new__ либо базовый PyType_GenericNew()
    .tp_init = (initproc) MyClass_init,                      // реализация __init__
    .tp_dealloc = (destructor) MyClass_dealloc,              // деструктор
    .tp_members = MyClass_members,                           // пользовательские поля
    .tp_methods = MyClass_methods,                           // пользовательские методы
};

#endif /* !Test_CLASS_H */
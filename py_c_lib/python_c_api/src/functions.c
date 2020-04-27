#ifndef Test_FUNCTIONS_H

#define PY_SSIZE_T_CLEAN
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

    // Проверяем на исключения
    if (strlen(str) < 10) {
        PyErr_SetString(StringTooShortError, "String length must be greater than 10");
        return NULL;
    }

    FILE *fp = fopen(filename, "w");
    bytes_copied = fputs(str, fp);
    fclose(fp);

    return PyLong_FromLong(bytes_copied);
}

#endif /* !Test_FUNCTIONS_H */
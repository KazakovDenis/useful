"""Создание Python-библиотеки на Си
https://realpython.com/build-python-c-extension-module/

Установить библиотеку:        python setup.py install
Упаковать библиотеку в архив: python setup.py sdist

Подготовка библиотеки к загрузке в PyPi:
https://packaging.python.org/tutorials/packaging-projects/
необходимо обновить:
    pip install --upgrade setuptools wheel
и выполнить:
    python setup.py sdist bdist_wheel
"""
from setuptools import find_packages, setup, Extension


with open("README.md", "r") as f:
    long_description = f.read()


my_ext = Extension(
    "my_ext",                # должно совпадать с PyInit_[c_extension] и названием в PyModuleDef
    ["src/mylib.c"],         # Unix-like
)


def main():

    setup(name="mypack",                                                       # имя для pip
          version="1.0.0",
          author="Author Name",
          author_email="mail@mail.com",
          url='https://github.com/user/exampleproject',
          license='MIT',

          description='Python interface for C functions',
          long_description=long_description,
          long_description_content_type="text/markdown",

          packages=find_packages(),                                            # ищет все __init__.py
          ext_package='mypackage/ext',                                         # куда положить с-расширение
          ext_modules=[my_ext],
          classifiers=[
              "Programming Language :: Python :: 3",
              "License :: OSI Approved :: MIT License",
              "Operating System :: OS Independent",
          ],
          python_requires='>=3.6',
    )


if __name__ == "__main__":
    main()

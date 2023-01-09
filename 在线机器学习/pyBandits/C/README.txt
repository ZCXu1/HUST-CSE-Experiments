Fast C versions of the utilities in kullpack.py
...............................................


To create the module use

> python setup.py build

the compiled module (.so file) will appear in build/lib.???
(typically yoursys-yourarch-yourversion).


Temporary files in build/temp.* can be removed with

> python setup.py clean


Building requires the header files and static library, typically
available in a package called python-dev (on Linux systems)

all: clean build install clean

build: kullback.c setup.py
	python setup.py build

install:
	\cp build/lib*/kullback.* ../

clean: setup.py
	python setup.py clean

To comagicle the python extension put your 'magic_api.h' header file in this
folder and type the following command in the terminal:

python setup.py build_ext -b .

If the comagiclation succeeds a new file will be created: 'pyMagic.so'.
This extension presents four functions that call your new system calls:
1) get_wand
2) attack
3) legilimens
4) list_secrets

You can use this functions in a python script or directly from the python
interpreter, type 'python' in the terminal and then the following commands:

>>>import pyMagic
>>>import os
>>>pyMagic.get_wand()

The syntax of the command can be found by typing the following in the python
interpreter:

>>>import pyMagic
>>>help(pyMagic.legilimens)

You can also use the ipython interpreter (you can find the rpm package in the
course website). After running ipython (type 'ipython' in the terminal) do:

[1] import pyMagic
[2] pyMagic.attack?

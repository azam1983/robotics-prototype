## Dependencies and other stuff to watch out for when testing
To make this work make sure you have python2 and gtk3 installed.
This program uses `commands` package which is deprecated in python 3.
Make sure `pyserial` is installed using pip. 
You will also need to make sure that you are using python-gi, and not gi library.
Make sure to remove the apt-get gi library if you have it installed.
Also you will need to run ArduinoSerialReceiver.py with sudo.

Some potentially helpful commands may include:
`sudo pip uninstall gi`
`sudo pip install pyserial`

# robotics-prototype
This repo contains the beginning of the new (2017-2018) Robotics software team code.

## Getting Started
Firstly, this project is built in Python 3.3+. You need to have a version of Python installed that is 3.3+. Make sure that whenever you use `python` or `python3` or whatever later on meets this requirement.

Secondly, it is imperative you use a virtual env (instead of your system Python) to use/contribute to the project. 

### Setup [venv](https://docs.python.org/3.6/library/venv.html#module-venv)
Navigate to the projects root directory (`cd ~/.../robotics-prototype`) and create the virtual environment (this command won't work if you call using Python 2+ since `venv` was introduced only in 3.3+):
```
$ python3 -m venv ./venv
```
Note: your actual alias for calling Python might be different (e.g. `python`, `py`, `py3`, etc). You can ensure whichever it might be is version 3+ by typing `which python` in Linux/Mac (bash/zsh) or `where py` in Windows (cmd.exe).

You should see a new directory named `venv` inside the root directory. We can now activate this virtual environment.


Linux/Mac (bash/zsh):
```
$ source venv/bin/activate
```
Windows (cmd.exe):
```
C:\> venv\Scripts\activate.bat
```

You should see a `(venv)` appear at the beginning of your terminal prompt (in Linux and Mac at least) indicating that you are working inside the virtualenv. Now when you install something:
```
(venv) $ pip install <package>
```
It will get installed in the `venv`, and not conflict with other projects installed system-wide.

To leave the virtual environment run.


Linux/Mac (bash/zsh):
```
(venv) $ deactivate
```
Windows (cmd.exe):
```
C:\> deactivate.bat
```

### Install [dependencies](https://pip.pypa.io/en/stable/user_guide/#requirements-files)
First, ensure you activated your `venv` and you are in the root project directory. Then, install the required packages:
```
pip install -r requirements.txt
```

### Setup [setuptools](https://setuptools.readthedocs.io/en/latest/setuptools.html#development-mode)
When you try to run `pytest` from anywhere inside the project, there's a very good chance you'll get `ModuleNotFoundError` thrown. This is because absolute imports inside each test file won't work if the test is being executed by `pytest` inside the test's own directory (i.e. the `sys.path` variable will only contain the directory where the test file lives, not the root directory). To encourage best practices, and avoid doing dirty/hacky `sys.path` manipulation within each python file, using `setuptools`'s **develop** feature will allow the virtual environment to create temporary dev package installs using "hooks" (eggs) to each path inside package directory as defined by the "name" attribute in `setup.py` (i.e. asimov will be available as though it was installed as a package through `pip`). 

In other words, this means all imports of modules inside the `asimov` directory should be imported with absolute path, e.g. inside `"tests/unit/test_motor.py"`, the `Motor` class can be imported using:
```
# tests/unit/test_motor.py
from asimov.basestation.Motor import Motor
...
```

First we have to install `setuptools` (make sure you activated your `venv`). In root project directory:
```
$ pip install setuptools
```

Still in root project directory (which contains `setup.py`):
```
$ python setup.py develop
```

You should now be able to execute tests without `ModuleNotFoundError` thrown:
```
$ pytest
```

To remove dev packages you just installed using `setuptools`:
```
$ python setup.py develop --uninstall
```

**DISCLAIMER:** This issue with module imports via `pytest` was the motivating factor to change the project directory structure. For this technique to work, the 'source' code must live inside (nested) a main directory (usually named the same as project directory name or other suitable representative identifier such as **asimov** in this case). The `src` subdirectory was removed alltogether because it made no sense when importing a package module by name like `import asimov.src.basestation.Motor`, which has no meaning/place in a module semantic context. Most Python projects do not use a `src` directory unless it's for storing their source code that eventually gets compiled to binary (i.e. such as `.c`, `.h`, etc.. files). Also, `base-station` was renamed to `basestation` because Python no-likey dashes in import statements.

## ODROID
### How to upload Arduino scripts from the odroid

1. Make sure the arduino is plugged into the odroid

2. Copy your Arduino source(s) into platformio/src/

3. Navigate to platformio/ folder

4. Upload the script via the following command: `platformio run -t upload`. This will both compile and upload the code.

Note: I didn't look into adding libraries yet but I'm pretty sure you want to place them in the platformio/lib folder. See [platformio lib help page](http://docs.platformio.org/en/latest/userguide/lib/index.html) to learn more

### IP emailer service

The odroid will send an email with its local IP address every time it boots

This was accomplished by running `syncEmailer.sh` and adding the following line to the crontab via `crontab -e`:

```
@reboot /home/odroid/emailer/runEmailer.sh
```

Let Peter or David know if you want to be added to this mailing list.

### Remote connect from home

- Open a terminal (I recommend git bash if you're using windows)
- SSH into Concordia's network with your netname (type the following into the terminal): 
```
ssh net_name@login.encs.concordia.ca
```
- It should ask you for a password, which will your ENCS password
- Grab the latest IP address of the odroid from your email, then ssh into it: 
```
ssh odroid@ip_address
```
- It should ask you for a password, which will be `odroid`


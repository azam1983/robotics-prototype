#!/usr/bin/env python
'''
To witness the full effect of this test, make sure that you have an arduino connected via USB to your computer
which is outputting periodically via Serial.println statements and make sure to also run SingleLineArduinoLogger.py in the background
The ArduinoTester.py sketch was used with this example. This program also currently assumes that the log file is named "test.txt"
To demo this program upload the ArduinoTester.ino sketch in this directory to your Arduino
'''

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GObject
import os
import subprocess
import commands

class ArduinoSerialReceiver(Gtk.Window):
	def __init__(self):
		Gtk.Window.__init__(self, title="Arduino Serial Output Receiver Demo")

		self.arduinoLoggerOn = False # initial assumption

		# creates a box container in which you can add widgets (such as buttons, labels, etc.)
		self.box = Gtk.Box(spacing=6)
		self.add(self.box)

		if os.path.isfile("./test.txt"):
			self.file = open("test.txt", "r+")
		else: # handle case where file DNE
			self.file = open("test.txt", "w+")
			self.file.close();
			self.file = open("test.txt", "r+")

		self.gracefulExitBtn = Gtk.Button(label="Graceful Exit")
		self.gracefulExitBtn.connect("clicked", self.terminate)
		self.box.pack_start(self.gracefulExitBtn, True, True, 0)

		self.startLoggerBtn = Gtk.Button(label="Start logging Arduino Output")
		self.startLoggerBtn.connect("clicked", self.start_logging)
		self.box.pack_start(self.startLoggerBtn, True, True, 0)

		self.label1 = Gtk.Label(label="Wait for it...")
		self.box.pack_start(self.label1, True, True, 0)

		# if the callback method does not return True then this will only call it once
		GObject.timeout_add(1000, self.read_and_update_once) # interval set to 1 second, based of the Arduino test program outputting every second

	# reads the first line of the file to which the arduino is logging it's data
	# and sets the text of the label to that line
	def read_and_update_once(self):
		self.file.seek(0)
		readData = self.file.read()

		if readData == "":
			self.label1.set_text("empty file")
		else:
			self.label1.set_text(readData)

		return True

	# ideally, we want this logging process to begin as soon as the GUI is started
	# (IOW as soon as the connection to the arm is established, on startup)
	def start_logging(self, widget):
		# check if it is currentlyrunning
		result = commands.getstatusoutput('pgrep -f SingleLineArduinoLogger.py')

		if "\\n" in str(result): # strange but if contains "\n" in string, i.e. 2 pids, then it is already running 
			print("SingleLineArduinoLogger.py running")
			self.arduinoLoggerOn = True
		else:
			self.arduinoLoggerOn = False

		if self.arduinoLoggerOn == False:
			subprocess.Popen(["python", "SingleLineArduinoLogger.py"])
			self.arduinoLoggerOn = True

	# slightly more graceful exit than default top right corner x button
	# can I map this to the right corner x button?
	def terminate(self, widget):
		self.file.close()

		if self.arduinoLoggerOn == True:
			print("Terminating SingleLineArduinoLogger.py process")
			bashCommand = "pkill -9 -f SingleLineArduinoLogger.py"
			output = subprocess.check_output(['bash','-c', bashCommand])
		Gtk.main_quit()

window = ArduinoSerialReceiver()
window.maximize()
window.connect("delete-event", Gtk.main_quit)
window.show_all()
Gtk.main()

"""
The purpose of this code is to show a camera feed displayed in
a GTK window. Note: If you are on a virtual environment and can't
import gi, install vext, and then use the command vext.gi in your
virtual environment to ge it to work.
"""

import cv2
import numpy as np
import gi

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GLib, GdkPixbuf

""""
 To capture video we need to create a video capture object.
 It's argument can take either be the device index device index or the name of a video file. 
 The device index is just the number to specify which camera to use. 
 If you have one camera connected, just pass 0. If you have more
 than one camera then you can select the second camera by passing 1
"""

cap = cv2.VideoCapture(0)

"""
 Here we are linking builder to a Glade file labled "test.glade"
 The reason we do this is so we can write functions and integrate it into the file
"""

builder = Gtk.Builder()
builder.add_from_file("test.glade")

# Use this variable to convert camera feed from black and white, into colour
greyscale = False


class Handler:
    def onDeleteWindow(self, *args):
        Gtk.main_quit(*args)

    def toggleGreyscale(self, *args):
        global greyscale
        greyscale = ~ greyscale


window = builder.get_object("window1")
image = builder.get_object("image")
window.show_all()
builder.connect_signals(Handler())

def show_frame(*args):
    ret, frame = cap.read()

    """
    Resize the video to twice its native resolution because I think it looks nicer. 
    If you want to change this, change the values on fx and fy
    """
    frame = cv2.resize(frame, None, fx=2, fy=2, interpolation=cv2.INTER_CUBIC)

    """
    To convert to greyscale, we have to convert BGR to greyscale and then greyscale to RGB. 
    GTK can apparently only work with the RGB colourspace and OpenCV natively generates BGR, not RGB.
     So if you want to display colour you need to do a conversion. That's what we're gonna do here
    """

    if greyscale:
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        frame = cv2.cvtColor(frame, cv2.COLOR_GRAY2RGB)
    else:
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    """
    To get the data into a form that GtkImage understands, we have to convert the numpy ndarray (stands for N-Dimensional)
    that openCV stores data into a byte array using .tostring() (this is how GtkImage processes data).
    We then use GdkPixbuf.Pixbuf.new_from_data to convert this to a pixbuf. The False argument is to say there is no alpha channel
    (an alphal channel is a color component that represents the degree of transparency of a color. If you don't understand, don't
    worry about it, I don't fully get it either lol xD). 8 is the only bit depth supported. frame.shape[1] is the 
    image width and frame.shape[2] is the image height,  and the last argument is the number of bytes in one row of the 
    image (ie. the number of channels times the width in pixels).
    """

    pb = GdkPixbuf.Pixbuf.new_from_data(frame.tostring(),
                                        GdkPixbuf.Colorspace.RGB,
                                        False,
                                        8,
                                        frame.shape[1],
                                        frame.shape[0],
                                        frame.shape[2] * frame.shape[1])

    """
    Apparently displaying the pixbuf directly require us to manually clean up the pixbuf object when we’re done with it 
    which is kinda annoying, but if you instead display a copy of it. This gets around the problem with
    the memory management.
    """

    image.set_from_pixbuf(pb.copy())
    return True


"""
The function show_frame gets called by the GTK idler. GLib.idle_add(show_frame) is adding the function to the list 
of functions called when idle.You have to return True from idle functions or they don’t get called again
which is done at the end of show_frame :)
"""

GLib.idle_add(show_frame)
Gtk.main()

"""
Note: Sorry for the hella long ccmments, but I figured if someone reading this wants to know what's
gucci with this code, I should give a good explanation. If you want to get a better understanding about the
things I talked about, check this out:

(For this one, ignore the crazy math and look and the functions, example: COLOR_BGR2GRAY)
Information about converting from BGR to RGB: https://docs.opencv.org/3.1.0/de/d25/imgproc_color_conversions.html

(Hit ctrl+F and search "new_from_data")
More about the GdkPixbuf.Pixbuf.new_from_data: https://lazka.github.io/pgi-docs/GdkPixbuf-2.0/classes/Pixbuf.html

(Go to example 8)
More about Pixbuf.new_from_data and memory: https://www.programcreek.com/python/example/81480/gi.repository.GdkPixbuf.Pixbuf
"""
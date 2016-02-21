KaNDAS is an interface to the Linux NDAS driver which allows to connect and disconnect NDAS devices. Some devices have multiple slots to connect to, these appear in KaNDAS as well.

## Installation ##

You need to have installed the packages _libqt4_ and _kdelibs4_ (or _libkde4_) and their development packages. Refer to your system's documentation for how to install these packages. The NDAS driver has to be installed, refer to [the Ximeta homepage](http://code.ximeta.com) for details. KaNDASd does also have to be installed, refer to UserguideKandasd for details.

Download a source package of the KaNDAS suite from this server and unpack it; a directory _kandas-0.2.0_ or similar should appear. Open a terminal window and go to the _kandas_ subdirectory, then execute the following commands:
```
mkdir build
cd build
cmake ..
make
sudo make install
```
If any of the commands fails (i.e. an error message does appear), do not proceed before the error has been fixed.

## Usage ##

Make sure a KaNDASd instance is running (that should be done automatically, refer to UserguideKandasd for details). Execute the command _kandas_ or open KaNDAS from the application's menu.

The KaNDAS main window contains a list view that shows all available devices, and the connection slots inside of these devices. Devices for home users only have one slot, so you do not have to worry about the difference.

If you have just installed the NDAS driver, you might want to add a NDAS device first. Use the toolbar button "Add device" to do so. You can find the credentials for your device in your device's documentation: The read access key (4 blocks of 5 characters each) is necessary to connect to the device at all. The write access key (5 characters) can optionally be entered to allow write access on this device.

Each device or slot may have an action button at the right, which allows to connect to or disconnect from this device. Let the mouse cursor rest on the button to get a tooltip about the action that is associated with this button.
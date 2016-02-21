KaNDASconsole is a suite of command line interfaces to the Linux NDAS driver which allows to connect and disconnect NDAS devices. Some devices have multiple slots to connect to, these can be managed with KaNDASconsole as well.

## Installation ##

You need to have installed the packages _libqt4_ and _kdelibs4_ (or _libkde4_) and their development packages. Refer to your system's documentation for how to install these packages. The NDAS driver has to be installed, refer to [the Ximeta homepage](http://code.ximeta.com) for details. KaNDASd does also have to be installed, refer to UserguideKandasd for details.

Download a source package of the KaNDAS suite from this server and unpack it; a directory _kandas-0.2.0_ or similar should appear. Open a terminal window and go to the _kandas-console_ subdirectory, then execute the following commands:
```
mkdir build
cd build
cmake ..
make
sudo make install
```
If any of the commands fails (i.e. an error message does appear), do not proceed before the error has been fixed.

## Usage ##

Make sure a KaNDASd instance is running (that should be done automatically, refer to UserguideKandasd for details).
  * The _kandas-info_ command gives an overview over the system state, state of devices, and of slots. By default, all available information is printed, use the -e, -d, and -s switches to override this.
  * The _kandas-up_ command connects a slot; you'll have to pass the slot number as an argument. (The slot number can be found in the output of _kandas-info_.) To connect the slot in read-only mode, use the -r switch. To connect a device instead of just one slot, use the -d switch and supply the device name instead of the slot number. (The device name can be found in the output of _kandas-info_, too.)
  * The _kandas-down_ command disconnects a slot (or device); it works exactly like _kandas-up_ (just without the -r switch, of course).
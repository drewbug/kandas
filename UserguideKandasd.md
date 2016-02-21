KaNDASd is an information management server for the Linux NDAS driver which allows to gather information about NDAS devices, connect to them, and add new devices. Some devices have multiple slots to connect to, these are managed by KaNDASd as well.

## Installation ##

You need to have installed the packages _libqt4_ and _kdelibs4_ (or _libkde4_) and their development packages. Refer to your system's documentation for how to install these packages. The NDAS driver has to be installed, refer to [the Ximeta homepage](http://code.ximeta.com) for details.

Download a source package of the KaNDAS suite from this server and unpack it; a directory _kandas-0.2.0_ or similar should appear. Open a terminal window and go to the _kandasd_ subdirectory, then execute the following commands:
```
mkdir build
cd build
cmake ..
make
sudo make install
```
If any of the commands fails (i.e. an error message does appear), do not proceed before the error has been fixed.

## Usage ##

KaNDASd can only be used in combination with a user interface such as KaNDAS (see UserguideKandas) oder KaNDASconsole (see UserguideKandasconsole). To check whether KaNDAS is running, run the following command in a terminal window:
```
qdbus --system org.kandas / org.kandas.daemonVersion
```

You will usually want to start KaNDASd on system boot. KaNDASd comes with an init.d script, refer to your distribution's documentation. (For example, openSUSE's YaST has a runlevel editor where you can auto-enable KaNDASd.)

Experts: To start KaNDASd manually, open a root terminal (or work with sudo in a normal terminal) and run _kandasd --detach_. This starts KaNDASd as background service (the KaNDASd instance can be controlled from the terminal by omitting the --detach switch.
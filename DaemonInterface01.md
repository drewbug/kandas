KaNDASd has two main intentions: It abstracts the interfaces of the Ximeta NDAS driver (i.e. the /proc/ndas tree and the ndasadmin program), and (as it runs as root) allows clients to perform operations without root privileges. To fulfil these purposes, KaNDASd offers a D-Bus interface at the system bus, accessible at `org.kandas /`. This is the reference documentation for the methods and signals in this interface.

KaNDASd defines some state enumerations which are transferred over D-Bus as integers. Include the header `<kandasd/definitions.h>` to get these. A standard XML interface definition is available in the same folder as `org.kandas.xml`. When talking of the data type _string_ in this documentation, the data type which is actually used in your client depends on the libraries you're using. When using Qt and its QtDBus, _string_ means QString, for instance.

**Warning:** This page refers to the interface of KaNDASd 0.1. To find out more about the interface of the current KaNDASd version, refer to DaemonInterface.

## Information about the daemon ##

```
  //Methods:
    string daemonVersion();
    string interfaceVersion();
```

Check the interface version of the running KaNDASd instance as the interface may differ between versions. For stable versions and current development versions, such differences will be noted on this page. Example version strings are _0.1_ or _0.2_.

The daemon version is not of programmatic relevance. However, you can use it in your UI's about dialog, or whatever you deserve.

## Registration of a new client ##

```
  //Methods:
    void registerClient();
    void unregisterClient();
    void refreshData();
```

When clients are registered on the server using the **registerClient** function, KaNDASd will eventually start to automatically refresh its data structures in intervals of three seconds. If you need a data update but cannot wait for the next auto-update, call the **refreshData** function manually. For average use cases, a manual refresh should be unnecessary. When shutting down your client, always unregister it as KaNDASd will stop to schedule automatic refreshes once all clients are disconnected.

## Initiation of the client's data structures ##

```
  //Methods:
    void initClient();
  //Signals:
    void initEnvironmentInfo(int state);
    void initDeviceInfo(string device);
    void initSlotInfo(int slot, string device, int state);
    void initComplete();
  //Enumerations:
    enum EnvironmentState
    {
        UnknownEnvironment = 0,
        SaneEnvironment = 1,
        NoDriverFound = 10,
        NoAdminFound = 11
    };
    enum SlotState
    {
        Undetermined = 0,
        Disconnected = 10,
        Connected = 11,
        Disconnecting = 20,
        Connecting = 21
    };
```

When starting your client you'll need the current state of the NDAS driver. Call the **initClient** function and you'll receive a sequence consisting of an **initEnvironmentInfo** signal, some **initDeviceInfo** signals, some **initSlotInfo** signals, and a **initComplete** signal (in this order). The signals contain information about the environment, the devices (one **initDeviceInfo** signal describes one device), and the slots (again, one signal means one slot). If your client is a direct modifier, you can start your operations in the handler for the **initComplete** signal.

Note that any operations will be rejected by KaNDASd if the environment is not sane; any operations on a specific slot will be rejected if this slot has an undetermined state (or a state that makes the operation illogic; of course one cannot disconnect a disconnected slot). If operations fail or are rejected by KaNDASd, no specific error message will appear.

## Stay informed about changes ##

```
  //Signals:
    void environmentChanged(int state);
    void deviceAdded(string device);
    void deviceRemoved(string device);
    void slotAdded(int slot, string device, int state);
    void slotChanged(int slot, string device, int state);
    void slotRemoved(int slot, string device);
  //Enumerations
    enum EnvironmentState; //see above for full definition
    enum SlotState;        //see above for full definition
```

As noted earlier, KaNDASd does automatically scan the NDAS driver for changes. These are propagated to clients using these signals. Note that if a device with slots is removed, the **deviceRemoved** signal is emitted before the **slotRemoved** signals. I recommend that you remove the slots with the **deviceRemoved** signal; you can then safely ignore the **slotRemoved** signals.

## Change the environment state ##

```
  //Methods:
    void startDriver();
    void stopDriver();
```

If any slots are connected, a call to **stopDriver** will fail without further notification. You should catch the **environmentChanged** signal to find out whether the operation was successful.

## Connect and disconnect ##

```
  //Methods:
    void connectDevice(string device, bool readOnly);
    void connectSlot(int slot, bool readOnly);
    void disconnectDevice(string device);
    void disconnectSlot(int slot);
```

The **connectDevice** and **disconnectDevice** functions simply call the according slot methods for all slots available at this device. If the connection fails for one slot (e.g. because it is already connected), the operation will not be aborted; each slot connection is independent in this process. Again, all functions can fail without notification. Catch the **slotChanged** signals to find whether the operation was successful.
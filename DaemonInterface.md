KaNDASd has two main intentions: It abstracts the interfaces of the Ximeta NDAS driver (i.e. the /proc/ndas tree and the ndasadmin program), and (as it runs as root) allows clients to perform operations without root privileges. To fulfil these purposes, KaNDASd offers a D-Bus interface at the system bus, accessible at `org.kandas /`. This is the reference documentation for the methods and signals in this interface.

KaNDASd defines some state enumerations which are transferred over D-Bus as integers. Include the header `<kandasd/definitions.h>` to get these. A standard XML interface definition is available in the same folder as `org.kandas.xml`. When talking of the data types _string_ and _stringlist_ in this documentation, the data type which is actually used in your client depends on the libraries you're using. When using Qt and its QtDBus, _string_ means `QString` and _stringlist_ means `QList<QString>`, for instance.

## Information about the daemon ##

```
  //Methods:
    string daemonVersion();
    string interfaceVersion();
```

Check the interface version of the running KaNDASd instance as the interface may differ between versions. Example version strings are _0.1_ or _0.2_. This page documents the interface version _0.2_.

The daemon version is not of programmatic relevance. However, you can use it in your UI's about dialog if you want.

## Registration of a new client ##

```
  //Methods:
    void registerClient();
    void unregisterClient();
```

When clients are registered on the server using the **registerClient** function, KaNDASd will start to automatically refresh its data structures in intervals of two seconds. When shutting down your client, always unregister it as KaNDASd will stop to schedule automatic refreshes once all clients are disconnected.

## Upkeep of client data structures ##

```
  //Signals:
    void systemInfo(int state);
    void deviceInfo(string device, string serial, int state, bool writable);
    void slotInfo(int slot, string device, string blockDevice, int state);
    void deviceRemoved(string device);
    void slotRemoved(int slot);
    void initComplete();
  //Enumerations:
    enum Kandas::SystemState
    {
        SystemUnchecked = 0,
        SystemChecked = 1,
        NoDriverFound = 10,
        NoAdminFound
    };
    enum Kandas::DeviceState
    {
        DeviceOffline = 0,
        DeviceOnline = 1,
        DeviceConnectionError = 10,
        DeviceLoginError = 11
    };
    enum Kandas::SlotState
    {
        SlotOffline = 0,
        DisconnectedSlot = 1,
        ConnectedSlot,
        DisconnectingSlot = 10,
        ConnectingSlot
    };
```

KaNDASd automatically scans the NDAS driver regularly for changes. These are propagated to clients through a sequence of **systemInfo**, **deviceInfo** and **slotInfo** signals (in this order). Note that the complete information is sent in every update round, even if nothing has changed. The state that is given with every signal is always a value of one of the three enumerations above. For example, the state that comes with a **deviceInfo** signal can safely be casted into a `Kandas::DeviceState` value.

A data update is scheduled when a new client registers with the **registerClient** method. After the next update, KaNDASd will send a **initComplete** signal to indicate that the client has recieved all data it might need.

Any further operations will be rejected by KaNDASd if the system state is not checked or invalid; any operations on a specific slot will be rejected if this slot is offline, or when the state makes the operation illogic (of course one cannot disconnect a disconnected slot). KaNDASd will not send an error message when operations fail or are rejected.

## Connection management ##

```
  //Methods:
    void connectDevice(string device, bool readOnly);
    void connectSlot(int slot, bool readOnly);
    void disconnectDevice(string device);
    void disconnectSlot(int slot);
```

The **connectDevice** and **disconnectDevice** functions simply call the according slot methods for all slots available at this device. If the connection fails for one slot (e.g. because it is already connected), the operation will not be aborted; each slot connection is independent in this process. As noted earlier, all functions can fail without notification. Catch the **slotChanged** signals to find whether the operation was successful.

## Device management ##

```
  //Methods:
    int addDevice(string device, stringlist readKey, string writeKey);
    void removeDevice(string device);
  //Enumerations:
    enum Kandas::AddDeviceResult
    {
        DeviceAdded = 0,
        DeviceAdditionFailed = 1,
        InvalidDeviceKey,
        InvalidDeviceName,
        DeviceExistsAlready
    };
```

Use these methods to register or unregister devices. The **addDevice** method returns a value from the `AddDeviceResult` enumeration, that may indicate a failure reason. For example, the operation fails if the given keys are invalid, or if a device with this name does already exist. You can prevent most of these failure reasons with the following checks:

  * The device name may not contain slashes or blank spaces. (All other UTF-8 characters should be possible.)
  * The readKey has to be a list of 4 key blocks, and the writeKey (optional!) has to be a single key block. Each key block has to consist of 5 ASCII alphanumeric characters.
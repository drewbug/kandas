/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "engine.h"
#include "device.h"
#include "kandasadaptor.h"

#include <QDir>
#include <QFile>
#include <KDebug> //do not remove, is needed by kError and kWarning
#include <KProcess>

//instance functions

QString Kandas::Daemon::Engine::InformationSourceDirectory;

Kandas::Daemon::Engine::Engine()
    : QObject()
    , m_clean(true)
    , m_clientCount(0)
    , m_system(Kandas::SystemUnchecked)
{
    //auto refresh
    connect(&m_autoRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshData()));
    //D-Bus
    new KandasAdaptor(this);
    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.registerService("org.kandas"))
    {
        kError() << "Could not register service. D-Bus returned:" << bus.lastError().message();
        m_clean = false;
    }
    else
        bus.registerObject("/", this);
    //schedule initial data updates
    QTimer::singleShot(0, this, SLOT(refreshData()));
}

bool Kandas::Daemon::Engine::clean() const
{
    return m_clean;
}

//daemon information

QString Kandas::Daemon::Engine::daemonVersion()
{
    return Kandas::Daemon::DaemonVersion;
}

QString Kandas::Daemon::Engine::interfaceVersion()
{
    return Kandas::Daemon::InterfaceVersion;
}

//client management

void Kandas::Daemon::Engine::registerClient()
{
    ++m_clientCount;
    //make sure the client has up-to-date data
    refreshData(); //systemInfo(), slotInfo(), and clientInfo() signals are now sent out
    m_autoRefreshTimer.start(Kandas::Daemon::RefreshInterval);
    emit initComplete();
}

void Kandas::Daemon::Engine::unregisterClient()
{
    if (--m_clientCount <= 0)
    {
        m_clientCount = 0;
        m_autoRefreshTimer.stop();
    }
}

//NDAS management

bool validateKeyBlock(const QString &keyBlock, bool allowEmpty = false)
{
    const int length = keyBlock.length();
    if (allowEmpty && length == 0)
        return true;
    if (length != 5)
        return false;
    for (int i = 0; i < 5; ++i)
    {
        const QChar character = keyBlock.at(i);
        //only accept ASCII letters or arabic numbers
        if (!character.isLetterOrNumber() || character.unicode() >= 128)
            return false;
    }
    return true;
}

int Kandas::Daemon::Engine::addDevice(const QString &deviceName, const QList<QString> &readKey, const QString &writeKey)
{
    //only register new device if no device with this name exists
    if (m_devices.device(deviceName))
        return Kandas::DeviceExistsAlready;
    //validate input
    if (deviceName.contains('/'))
        return Kandas::InvalidDeviceName;
    if (readKey.count() != 4)
        return Kandas::InvalidDeviceKey;
    foreach (const QString &keyBlock, readKey)
        if (!validateKeyBlock(keyBlock))
            return Kandas::InvalidDeviceKey;
    if (!validateKeyBlock(writeKey, true))
        return Kandas::InvalidDeviceKey;
    //build key string
    QString keyString = QStringList(readKey).join(QChar('-'));
    if (!writeKey.isEmpty())
        keyString += '-' + writeKey;
    //call ndasadmin
    QStringList args; args << "register" << keyString << "-n" << deviceName << "-b";
    KProcess process;
    process.setProgram("ndasadmin", args);
    process.setOutputChannelMode(KProcess::OnlyStderrChannel);
    process.start();
    process.waitForFinished();
    const QString errorOutput = QString::fromUtf8(process.readAllStandardError()).simplified();
    if (errorOutput.isEmpty())
        return Kandas::DeviceAdded;
    else if (errorOutput.contains(QLatin1String("register: invalid NDAS ID.")))
        return Kandas::InvalidDeviceKey;
    else
        return Kandas::DeviceAdditionFailed;
    //
}

void Kandas::Daemon::Engine::removeDevice(const QString &deviceName)
{
    //unegister device only if a device with this name exists
    if (!m_devices.device(deviceName))
        return;
    //call ndasadmin
    QStringList args; args << "unregister" << "-n" << deviceName;
    KProcess::startDetached("ndasadmin", args);
}

void Kandas::Daemon::Engine::connectSlot(int slotNumber, bool readOnly)
{
    //check environment and slot state
    if (m_system != Kandas::SystemChecked)
        return;
    Kandas::Daemon::Slot* slot = m_slots.slot(slotNumber);
    if (!slot)
        return;
    if (slot->state() != Kandas::DisconnectedSlot)
        return;
    //update slot state
    slot->setState(Kandas::ConnectingSlot);
    emit slotInfo(slotNumber, slot->deviceName(), slot->blockDeviceName(), slot->state());
    //call ndasadmin
    QStringList args; args << "enable" << "-s" << QString::number(slotNumber);
    if (readOnly)
        args << "-o" << "r";
    else
        args << "-o" << "w";
    KProcess::startDetached("ndasadmin", args);
}

void Kandas::Daemon::Engine::disconnectSlot(int slotNumber)
{
    //check environment and slot state
    if (m_system != Kandas::SystemChecked)
        return;
    Kandas::Daemon::Slot* slot = m_slots.slot(slotNumber);
    if (!slot)
        return;
    if (slot->state() != Kandas::ConnectedSlot)
        return;
    //update slot state
    slot->setState(Kandas::DisconnectingSlot);
    emit slotInfo(slotNumber, slot->deviceName(), slot->blockDeviceName(), slot->state());
    //call ndasadmin
    QStringList args; args << "disable" << "-s" << QString::number(slotNumber);
    KProcess::startDetached("ndasadmin", args);
}

void Kandas::Daemon::Engine::connectDevice(const QString &device, bool readOnly)
{
    foreach (Kandas::Daemon::Slot* slot, m_slots)
        if (slot->deviceName() == device)
            connectSlot(slot->number(), readOnly);
}

void Kandas::Daemon::Engine::disconnectDevice(const QString &device)
{
    foreach (Kandas::Daemon::Slot* slot, m_slots)
        if (slot->deviceName() == device)
            disconnectSlot(slot->number());
}

void Kandas::Daemon::Engine::refreshData()
{
    //flush device and slot lists, but keep lists of the device names and slot numbers in order to emit the deviceRemoved/slotRemoved signals later
    Kandas::Daemon::DeviceList removedDevices(m_devices);
    Kandas::Daemon::SlotList removedSlots(m_slots);
    m_devices.clear();
    m_slots.clear();
    //this will be of use when reading files
    const int bufferSize = 1024; char buffer[bufferSize];
    ////////////////////////////////////////
    // determine state of driver          //
    ////////////////////////////////////////
    m_system = Kandas::SystemUnchecked;
    //check availability of NDAS driver through existence of information source directory (cannot check kernel modules because that would be incompatible to KaNDASsimul)
    static const QDir dir(InformationSourceDirectory);
    if (!dir.exists())
        m_system = Kandas::NoDriverFound;
    //check availability of ndasadmin program
    if (m_system == Kandas::SystemUnchecked)
    {
        static const QString whichProgram = QLatin1String("which");
        static const QStringList whichArgs(QLatin1String("ndasadmin"));
        KProcess process;
        process.setProgram(whichProgram, whichArgs);
        process.setOutputChannelMode(KProcess::OnlyStdoutChannel);
        process.start();
        process.waitForFinished();
        if (QString::fromUtf8(process.readAllStandardOutput()).simplified().isEmpty())
            m_system = Kandas::NoAdminFound;
    }
    //system is sane if no regression has been found
    if (m_system == Kandas::SystemUnchecked)
        m_system = Kandas::SystemChecked;
    emit systemInfo(m_system);
    ////////////////////////////////////////
    // update list of devices             //
    ////////////////////////////////////////
    if (m_system == Kandas::SystemChecked)
    {
        //open devices list
        const QString devicesListPath = QString("%1/devs").arg(InformationSourceDirectory);
        QFile devicesListFile(devicesListPath);
        if (devicesListFile.exists() && devicesListFile.open(QIODevice::ReadOnly) && devicesListFile.isReadable())
        {
            //read devices list
            devicesListFile.readLine(buffer, bufferSize); //skip first line (human-readable captions)
            while (devicesListFile.readLine(buffer, bufferSize) != -1)
            {
                const QString metadataLine = QString::fromUtf8(buffer).simplified();
                Kandas::Daemon::Device* device = new Kandas::Daemon::Device(metadataLine);
                m_devices << device;
                //this device is available - the assumption that it was removed is incorrect
                if (removedDevices.contains(device->name()))
                {
                    Kandas::Daemon::Device* oldDevice = removedDevices.device(device->name());
                    removedDevices.removeAll(oldDevice);
                    delete oldDevice;
                }
            }
            devicesListFile.close();
        }
    }
    ////////////////////////////////////////
    // update list of slots               //
    ////////////////////////////////////////
    if (m_system == Kandas::SystemChecked)
    {
        foreach (Kandas::Daemon::Device* device, m_devices)
        {
            QList<int> slotNumbers = device->getSlotNumbers();
            foreach (int slotNumber, slotNumbers)
            {
                //create slot (and read information for slot)
                Kandas::Daemon::Slot* slot = new Kandas::Daemon::Slot(device->name(), slotNumber);
                m_slots << slot;
                //this slot is available - the assumption that it was removed is incorrect
                if (removedSlots.contains(slotNumber))
                {
                    Kandas::Daemon::Slot* oldSlot = removedSlots.slot(slotNumber);
                    slot->setPreviousState(oldSlot->state());
                    removedSlots.removeAll(oldSlot);
                    delete oldSlot;
                }
            }
        }
    }
    ////////////////////////////////////////
    // propagate information              //
    ////////////////////////////////////////
    foreach (Kandas::Daemon::Device* oldDevice, removedDevices)
    {
        emit deviceRemoved(oldDevice->name());
        delete oldDevice;
    }
    foreach (Kandas::Daemon::Device* device, m_devices)
        emit deviceInfo(device->name(), device->serial(), device->state(), device->hasWriteKey());
    foreach (Kandas::Daemon::Slot* oldSlot, removedSlots)
    {
        emit slotRemoved(oldSlot->number());
        delete oldSlot;
    }
    foreach (Kandas::Daemon::Slot* slot, m_slots)
        emit slotInfo(slot->number(), slot->deviceName(), slot->blockDeviceName(), slot->state());
}

#include "engine.moc"

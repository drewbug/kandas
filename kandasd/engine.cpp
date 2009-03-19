/***************************************************************************
 *   Copyright (C) 2008 Stefan Majewsky <majewsky@gmx.net>
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
#include "kandasadaptor.h"

#include <QDir>
#include <QFile>
#include <KDebug> //do not remove, is needed by kError and kWarning
#include <KProcess>

//singleton functions

Kandas::Daemon::Engine::Engine(const QString &infoSourceDir)
    : QObject()
    , m_clean(true)
    , m_infoSourceDir(infoSourceDir)
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
    m_clientCount++;
    //make sure the client has up-to-date data
    refreshData(); //systemInfo(), slotInfo(), and clientInfo() signals are now sent out
    m_autoRefreshTimer.start(Kandas::Daemon::RefreshInterval);
    emit initComplete();
}

void Kandas::Daemon::Engine::unregisterClient()
{
    m_clientCount--;
    if (m_clientCount <= 0)
    {
        m_clientCount = 0;
        m_autoRefreshTimer.stop();
    }
}

//NDAS management

void Kandas::Daemon::Engine::connectSlot(int,bool) //(int slot, bool readOnly)
{
}

void Kandas::Daemon::Engine::disconnectSlot(int) //(int slot)
{
}

void Kandas::Daemon::Engine::connectDevice(const QString &,bool) //(const QString &device, bool readOnly)
{
}

void Kandas::Daemon::Engine::disconnectDevice(const QString &) //(const QString &device)
{
}

void Kandas::Daemon::Engine::refreshData()
{
    ////////////////////////////////////////
    // determine state of driver          //
    ////////////////////////////////////////
    Kandas::SystemState newSystem = Kandas::SystemUnchecked;
    //check availability of NDAS driver through existence of information source directory (cannot check kernel modules because that would be incompatible to KaNDASsimul)
    static const QDir dir(m_infoSourceDir);
    if (!dir.exists())
        newSystem = Kandas::NoDriverFound;
    //check available of ndasadmin program
    if (newSystem == Kandas::SystemUnchecked)
    {
        static const QString whichProgram = QLatin1String("which");
        static const QStringList whichArgs(QLatin1String("ndasadmin"));
        KProcess process;
        process.setProgram(whichProgram, whichArgs);
        process.setOutputChannelMode(KProcess::OnlyStdoutChannel);
        process.start();
        process.waitForFinished();
        if (QString::fromUtf8(process.readAllStandardOutput()).simplified().isEmpty())
            newSystem = Kandas::NoAdminFound;
    }
    //system is sane if no regression has been found
    if (newSystem == Kandas::SystemUnchecked)
        newSystem = Kandas::SystemChecked;
    m_system = newSystem;
    emit systemInfo(newSystem);
    //if system is not sane, flush devices and slot list and do not continue further tests
    if (newSystem != Kandas::SystemChecked)
    {
        QHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
        while (iterSlots.hasNext())
            emit slotRemoved(iterSlots.next().key());
        m_slots.clear();
        foreach (QString device, m_devices)
            emit deviceRemoved(device);
        m_devices.clear();
        return;
    }
    ////////////////////////////////////////
    // update list of devices             //
    ////////////////////////////////////////
    //save copy of old devices list - by now, we have to assume that all devices have been removed
    QList<QString> removedDevices(m_devices);
    m_devices.clear();
    //open devices list
    static const QString devicesListPath = QString("%1/devs").arg(m_infoSourceDir);
    QFile devicesListFile(devicesListPath);
    if (!devicesListFile.exists() || !devicesListFile.open(QIODevice::ReadOnly) || !devicesListFile.isReadable())
    {
        //devices list could not be opened - assume that there are no devices and slots, leave lists empty and tell that everything has been removed
        QHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
        while (iterSlots.hasNext())
            emit slotRemoved(iterSlots.next().key());
        m_slots.clear();
        foreach (QString device, removedDevices)
            emit deviceRemoved(device);
        return;
    }
    //read devices list
    char buffer[1024];
    const int bufferSize = sizeof(buffer);
    devicesListFile.readLine(buffer, bufferSize); //skip first line (human-readable captions)
    static const QRegExp whitespace("\\s+");
    while (devicesListFile.readLine(buffer, bufferSize) != -1)
    {
        QString deviceName = QString::fromUtf8(buffer).simplified().section(whitespace, 0, 0);
        m_devices << deviceName;
        removedDevices.removeAll(deviceName); //this device is available - the assumption that it was removed is incorrect
    }
    devicesListFile.close();
    //propagate information
    foreach (QString device, removedDevices)
        emit deviceRemoved(device);
    foreach (QString device, m_devices)
        emit deviceInfo(device);
    ////////////////////////////////////////
    // update list of slots               //
    ////////////////////////////////////////
    //save copy of old slot list - by now, we have to assume that all slots have been removed
    QHash<int, Kandas::SlotInfo> removedSlots(m_slots);
    m_slots.clear();
    //read slot numbers from device lists
    static const QString slotListPath = QString("%1/devices/%2/slots").arg(m_infoSourceDir);
    static const QString slotInfoPath = QString("%1/slots/%2/info").arg(m_infoSourceDir);
    foreach (QString device, m_devices)
    {
        QFile slotListFile(slotListPath.arg(device));
        if (!slotListFile.exists() || !slotListFile.open(QIODevice::ReadOnly) || !slotListFile.isReadable())
            continue;
        while (slotListFile.readLine(buffer, bufferSize) != -1)
        {
            const int slot = QString::fromUtf8(buffer).simplified().toInt();
            if (slot == 0) //means that an error occured during the conversion
                continue;
            //open slot info file
            QFile slotInfoFile(slotInfoPath.arg(slot));
            if (!slotInfoFile.exists() || !slotInfoFile.open(QIODevice::ReadOnly) || !slotInfoFile.isReadable())
                continue;
            slotInfoFile.readLine(buffer, bufferSize); //skip first line (human-readable captions)
            if (slotInfoFile.readLine(buffer, bufferSize) == -1)
                continue;
            const QString line = QString::fromUtf8(buffer).simplified();
            slotInfoFile.close();
            //is slot enabled?
            Kandas::SlotState state = (line.section(' ', 0, 0) == "Enabled") ? Kandas::Connected : Kandas::Disconnected;
            //transitional states (we mis-use the data in removedSlots for this)
            if (removedSlots.contains(slot))
            {
                Kandas::SlotState oldState = removedSlots[slot].state;
                if (oldState == Kandas::Connecting && state == Kandas::Disconnected || oldState == Kandas::Disconnecting && state == Kandas::Connected)
                    state = oldState;
            }
            //save information
            m_slots[slot] = Kandas::SlotInfo(device, state);
            removedSlots.remove(slot); //this slot is available - the assumption that it was removed is incorrect
        }
        slotListFile.close();
    }
    //propagate information
    QHashIterator<int, Kandas::SlotInfo> iterRemovedSlots(removedSlots);
    while (iterRemovedSlots.hasNext())
        emit slotRemoved(iterRemovedSlots.next().key());
    QHashIterator<int, Kandas::SlotInfo> iterSlots(m_slots);
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        emit slotInfo(iterSlots.key(), iterSlots.value().device, iterSlots.value().state);
    }
}

#include "engine.moc"
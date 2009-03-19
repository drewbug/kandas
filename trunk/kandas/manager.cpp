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

#include "manager.h"
#include "devicemodel.h"
#include "slotmodel.h"

#include <iostream>
#include <QTimer>
#include <KIcon>
#include <KLocalizedString>

Kandas::Client::Manager::Manager()
    : m_deviceModel(new Kandas::Client::DeviceModel(this))
    , m_slotModel(new Kandas::Client::SlotModel(this))
    , m_interface("org.kandas", "/", QDBusConnection::systemBus(), this)
    , m_connectionClean(true)
    , m_system(Kandas::SystemUnchecked)
{
    //check version
    QString version = m_interface.interfaceVersion().value();
    if (version.isEmpty()) //no KaNDASd instance running
    {
        std::cerr << i18n("ERROR: KaNDASd is not running.").toUtf8().data() << std::endl;
        m_connectionClean = false;
    }
    else if (version != "0.2")
    {
        std::cerr << i18n("ERROR: Unknown KaNDASd version \"%1\" detected.", version).toUtf8().data() << std::endl;
        m_connectionClean = false;
    }
    //connect interface
    if (m_connectionClean)
    {
        connect(&m_interface, SIGNAL(initComplete()), this, SLOT(initComplete()));
        connect(&m_interface, SIGNAL(systemInfo(int)), this, SLOT(changeSystem(int)));
        connect(&m_interface, SIGNAL(deviceInfo(const QString &)), this, SLOT(changeDevice(const QString &)));
        connect(&m_interface, SIGNAL(slotInfo(int, const QString &, int)), this, SLOT(changeSlot(int, const QString &, int)));
        connect(&m_interface, SIGNAL(deviceRemoved(const QString &)), this, SLOT(removeDevice(const QString &)));
        connect(&m_interface, SIGNAL(slotRemoved(int)), this, SLOT(removeSlot(int)));
        m_interface.registerClient();
        connect(m_deviceModel, SIGNAL(modelReset()), this, SLOT(resetDeviceSelection()));
    }
    else
        QTimer::singleShot(0, this, SLOT(initComplete()));
}

Kandas::Client::Manager::~Manager()
{
    m_interface.unregisterClient();
    delete m_deviceModel;
}

bool Kandas::Client::Manager::error() const
{
    return !m_connectionClean || m_system != Kandas::SystemChecked;
}

QVariant Kandas::Client::Manager::errorContent(int role) const
{
    if (!m_connectionClean)
    {
        switch (role)
        {
            case Qt::DisplayRole:
                return i18n("Could not connect to KaNDASd service");
            case Kandas::Client::ConnectionStatusRole:
                return i18n("Please check your installation.");
            case Qt::DecorationRole:
                return KIcon("dialog-cancel");
        }
    }
    else if (m_system != Kandas::SystemChecked)
    {
        switch (role)
        {
            case Qt::DisplayRole:
                switch ((int) m_system) //without (int), I get an absurd warning about missing handling of SaneEnvironment
                {
                    case Kandas::SystemUnchecked:
                        return i18n("Waiting for system check");
                    case Kandas::NoDriverFound:
                        return i18n("NDAS driver is not loaded");
                    case Kandas::NoAdminFound:
                        return i18n("NDAS admin program could not be found");
                }
            case Kandas::Client::ConnectionStatusRole:
                if (m_system == Kandas::SystemUnchecked)
                    return QString();
                else
                    return i18n("Please check your installation.");
            case Qt::DecorationRole:
                return KIcon("dialog-cancel");
        }
    }
    return QVariant();
}

Kandas::Client::DeviceModel *Kandas::Client::Manager::deviceModel() const
{
    return m_deviceModel;
}

Kandas::Client::SlotModel *Kandas::Client::Manager::slotModel() const
{
    return m_slotModel;
}

void Kandas::Client::Manager::changeSystem(int systemState)
{
    if (m_system != systemState)
    {
        m_system = (Kandas::SystemState) systemState;
        m_deviceModel->reset();
        m_slotModel->reset();
    }
}

void Kandas::Client::Manager::changeDevice(const QString &device)
{
    //insert device into list if not already added
    foreach (Kandas::Client::DeviceInfo info, m_devices)
    {
        if (info.device == device)
            return;
    }
    m_deviceModel->deviceAboutToBeAdded(m_devices.count() - 1);
    m_devices << DeviceInfo(device);
    m_deviceModel->deviceAdded();
    m_slotModel->deviceAdded(m_devices.count() - 1);
}

void Kandas::Client::Manager::changeSlot(int slot, const QString &device, int state)
{
    Kandas::SlotState slotState = (Kandas::SlotState) state;
    //search for device
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::DeviceInfo &deviceInfo = m_devices[d];
        if (deviceInfo.device == device)
        {
            //try to find slot (in case that it was just changed, not added)
            for (int s = 0; s < deviceInfo.slotList.count(); ++s)
            {
                Kandas::Client::SlotInfo &slotInfo = deviceInfo.slotList[s];
                if (slotInfo.slot == slot)
                {
                    slotInfo.state = slotState;
                    m_slotModel->slotChanged(d, s);
                    m_deviceModel->deviceChanged(d);
                    return;
                }
            }
            //slot not found - insert new one
            m_slotModel->slotAboutToBeAdded(d, deviceInfo.slotList.count());
            deviceInfo.slotList << Kandas::Client::SlotInfo(slot, slotState);
            m_slotModel->slotAdded(d);
            return;
        }
    }
    //device not found - insert new device and add this slot
    Kandas::Client::DeviceInfo newDeviceInfo(device);
    newDeviceInfo.slotList << Kandas::Client::SlotInfo(slot, slotState);
    m_deviceModel->deviceAboutToBeAdded(m_devices.count() - 1);
    m_devices << newDeviceInfo;
    m_deviceModel->deviceAdded();
    m_slotModel->deviceAdded(m_devices.count() - 1); //do not call slotAdded() as the device is definitely not selected
}

void Kandas::Client::Manager::removeDevice(const QString &device)
{
    for (int d = 0; d < m_devices.count(); ++d)
    {
        if (m_devices[d].device == device)
        {
            m_deviceModel->deviceAboutToBeRemoved(d);
            m_devices.removeAt(d);
            m_deviceModel->deviceRemoved();
            m_slotModel->deviceRemoved(d);
        }
    }
}

void Kandas::Client::Manager::removeSlot(int slot)
{
    //search for device
    for (int d = 0; d < m_devices.count(); ++d)
    {
        Kandas::Client::DeviceInfo &deviceInfo = m_devices[d];
        //try to find slot (in case that it was just changed, not added)
        for (int s = 0; s < deviceInfo.slotList.count(); ++s)
        {
            if (deviceInfo.slotList[s].slot == slot)
            {
                m_slotModel->slotAboutToBeRemoved(d, s);
                deviceInfo.slotList.removeAt(s);
                m_slotModel->slotRemoved(d);
            }
        }
    }
}

void Kandas::Client::Manager::initComplete()
{
    disconnect(&m_interface, SIGNAL(initComplete()), this, SLOT(initComplete()));
    if (m_connectionClean)
        emit initializationComplete(m_interface.daemonVersion());
    else
    emit initializationComplete(QString());
}

void Kandas::Client::Manager::selectedDeviceChanged(const QModelIndex &device)
{
    if (device.isValid())
        m_slotModel->changeSelectedDevice(device.row());
}

void Kandas::Client::Manager::resetDeviceSelection()
{
    m_slotModel->changeSelectedDevice(-1);
}

void Kandas::Client::Manager::connectDevice(const QString &device, bool readOnly)
{
    m_interface.connectDevice(device, readOnly);
}

void Kandas::Client::Manager::connectSlot(int slot, bool readOnly)
{
    m_interface.connectSlot(slot, readOnly);
}

void Kandas::Client::Manager::disconnectDevice(const QString &device)
{
    m_interface.disconnectDevice(device);
}

void Kandas::Client::Manager::disconnectSlot(int slot)
{
    m_interface.disconnectSlot(slot);
}

#include "manager.moc"

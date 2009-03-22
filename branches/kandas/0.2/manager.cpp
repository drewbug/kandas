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

#include "manager.h"
#include "ndasmodel.h"
#include "ndassystemmodel.h"

#include <iostream>
#include <QTimer>
#include <KLocalizedString>

Kandas::Client::Manager::Manager()
    : m_interface("org.kandas", "/", QDBusConnection::systemBus(), this)
    , m_model(new Kandas::Client::NdasModel)
    , m_systemModel(new Kandas::Client::NdasSystemModel)
{
    //check version
    QString version = m_interface.interfaceVersion().value();
    if (version.isEmpty()) //no KaNDASd instance running
    {
        m_systemModel->setConnectionError(i18n("KaNDASd is not running."));
        QTimer::singleShot(0, this, SLOT(initComplete()));
    }
    else if (version != QLatin1String("0.2"))
    {
        m_systemModel->setConnectionError(i18n("Unknown KaNDASd version \"%1\" detected.", version));
        QTimer::singleShot(0, this, SLOT(initComplete()));
    }
    //connect interface
    else
    {
        connect(&m_interface, SIGNAL(initComplete()), this, SLOT(initComplete()));
        connect(&m_interface, SIGNAL(systemInfo(int)), this, SLOT(systemChanged(int)));
        connect(&m_interface, SIGNAL(deviceInfo(const QString &, const QString &, int, bool)), this, SLOT(deviceChanged(const QString &, const QString &, int, bool)));
        connect(&m_interface, SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(slotChanged(int, const QString &, const QString &, int)));
        connect(&m_interface, SIGNAL(deviceRemoved(const QString &)), this, SLOT(deviceRemoved(const QString &)));
        connect(&m_interface, SIGNAL(slotRemoved(int)), this, SLOT(slotRemoved(int)));
        m_interface.registerClient();
        return;
    }
}

Kandas::Client::Manager::~Manager()
{
    m_interface.unregisterClient();
    delete m_model;
    delete m_systemModel;
}

Kandas::Client::NdasModel *Kandas::Client::Manager::model() const
{
    return m_model;
}

Kandas::Client::NdasSystemModel *Kandas::Client::Manager::systemModel() const
{
    return m_systemModel;
}

void Kandas::Client::Manager::systemChanged(int systemState)
{
    Kandas::SystemState state = (Kandas::SystemState) systemState;
    m_systemModel->setState(state);
    emit systemStateChanged(state); //lets the view change its model if necessary
}

void Kandas::Client::Manager::deviceChanged(const QString &device, const QString &serial, int state, bool hasWriteKey)
{
    m_model->updateDevice(device, serial, (Kandas::DeviceState) state, hasWriteKey);
}

void Kandas::Client::Manager::slotChanged(int slot, const QString &device, const QString &blockDevice, int state)
{
    m_model->updateSlot(slot, device, blockDevice, (Kandas::SlotState) state);
}

void Kandas::Client::Manager::deviceRemoved(const QString &device)
{
    m_model->removeDevice(device);
}

void Kandas::Client::Manager::slotRemoved(int slot)
{
    m_model->removeSlot(slot);
}

void Kandas::Client::Manager::initComplete()
{
    disconnect(&m_interface, SIGNAL(initComplete()), this, SLOT(initComplete()));
    if (m_systemModel->connectionError().isEmpty())
        emit initializationComplete(m_interface.daemonVersion());
    else
        emit initializationComplete(QString());
}

Kandas::AddDeviceResult Kandas::Client::Manager::addDevice(const QString &device, const QStringList &readKey, const QString &writeKey)
{
    return (Kandas::AddDeviceResult) m_interface.addDevice(device, readKey, writeKey).value();
}

void Kandas::Client::Manager::removeDevice(const QString &device)
{
    m_interface.removeDevice(device);
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

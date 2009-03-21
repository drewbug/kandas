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

#include <iostream>
#include <QTimer>
#include <KIcon>
#include <KLocalizedString>

Kandas::Client::Manager::Manager()
    : m_interface("org.kandas", "/", QDBusConnection::systemBus(), this)
    , m_connectionClean(true)
    , m_system(Kandas::SystemUnchecked)
    , m_model(new Kandas::Client::NdasModel)
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
        connect(&m_interface, SIGNAL(deviceInfo(const QString &, const QString &, int, bool)), this, SLOT(changeDevice(const QString &, const QString &, int, bool)));
        connect(&m_interface, SIGNAL(slotInfo(int, const QString &, const QString &, int)), this, SLOT(changeSlot(int, const QString &, const QString &, int)));
        connect(&m_interface, SIGNAL(deviceRemoved(const QString &)), this, SLOT(removeDevice(const QString &)));
        connect(&m_interface, SIGNAL(slotRemoved(int)), this, SLOT(removeSlot(int)));
        m_interface.registerClient();
    }
    else
        QTimer::singleShot(0, this, SLOT(initComplete()));
}

Kandas::Client::Manager::~Manager()
{
    m_interface.unregisterClient();
    delete m_model;
}

#if 0
//TODO: create a Kandas::NdasSystemModel that mimics this functionality

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

#endif

Kandas::Client::NdasModel *Kandas::Client::Manager::model() const
{
    return m_model;
}

void Kandas::Client::Manager::changeSystem(int systemState)
{
    if (m_system != systemState)
    {
        m_system = (Kandas::SystemState) systemState;
        //TODO: let model react on new system state
    }
}

void Kandas::Client::Manager::changeDevice(const QString &device, const QString &serial, int state, bool hasWriteKey)
{
    m_model->updateDevice(device, serial, (Kandas::DeviceState) state, hasWriteKey);
}

void Kandas::Client::Manager::changeSlot(int slot, const QString &device, const QString &blockDevice, int state)
{
    m_model->updateSlot(slot, device, blockDevice, (Kandas::SlotState) state);
}

void Kandas::Client::Manager::removeDevice(const QString &device)
{
    m_model->removeDevice(device);
}

void Kandas::Client::Manager::removeSlot(int slot)
{
    m_model->removeSlot(slot);
}

void Kandas::Client::Manager::initComplete()
{
    disconnect(&m_interface, SIGNAL(initComplete()), this, SLOT(initComplete()));
    if (m_connectionClean)
        emit initializationComplete(m_interface.daemonVersion());
    else
    emit initializationComplete(QString());
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

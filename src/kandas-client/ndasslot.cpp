/***************************************************************************
 *   Copyright 2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "ndasslot.h"
#include "ndasmodel.h"

#include <KIcon>
#include <KIconLoader>
#include <KLocalizedString>

Kandas::Client::NdasSlot::NdasSlot(int number, const QString &deviceName, const QString &blockDeviceName, Kandas::SlotState state)
    : m_number(number)
    , m_deviceName(deviceName)
    , m_blockDeviceName(blockDeviceName)
    , m_state(state)
{
}

Kandas::Client::NdasSlot::~NdasSlot()
{
    //only necessary to make the class polymorphic
}

void Kandas::Client::NdasSlot::update(const QString &deviceName, const QString &blockDeviceName, Kandas::SlotState state)
{
    m_deviceName = deviceName;
    m_blockDeviceName = blockDeviceName;
    m_state = state;
}

int Kandas::Client::NdasSlot::number() const
{
    return m_number;
}

QString Kandas::Client::NdasSlot::deviceName() const
{
    return m_deviceName;
}

QString Kandas::Client::NdasSlot::blockDeviceName() const
{
    return m_blockDeviceName;
}

Kandas::SlotState Kandas::Client::NdasSlot::state() const
{
    return m_state;
}

QVariant Kandas::Client::NdasSlot::data(int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return i18n("Connection point %1", m_number, m_deviceName);
        case Qt::DecorationRole:
            switch (m_state)
            {
                case Kandas::ConnectedSlot:
                case Kandas::DisconnectingSlot:
                    return KIcon("network-wired", KIconLoader::global(), QStringList() << "emblem-mounted");
                case Kandas::SlotOffline:
                case Kandas::DisconnectedSlot:
                case Kandas::ConnectingSlot:
                    return KIcon("network-wired");
            }
        case Kandas::Client::SecondDecorationRole:
            if (m_state == Kandas::DisconnectedSlot)
                return KIcon("media-playback-start");
            else if (m_state == Kandas::ConnectedSlot)
                return KIcon("media-eject");
            else //transitional state or offline
                return KIcon();
        case Kandas::Client::ActionRole:
            if (m_state == Kandas::DisconnectedSlot)
                return QLatin1String("connect-slot");
            else if (m_state == Kandas::ConnectedSlot)
                return QLatin1String("disconnect-slot");
            else //transitional state or offline
                return QString();
        case Kandas::Client::ActionDescriptionRole:
            if (m_state == Kandas::DisconnectedSlot)
                return i18n("Connect to this connection point");
            else if (m_state == Kandas::ConnectedSlot)
                return i18n("Disconnect from this connection point");
            else //transitional state or offline
                return QString();
        case Kandas::Client::SecondDisplayRole:
            switch (m_state)
            {
                case Kandas::SlotOffline:
                    return i18n("Offline");
                case Kandas::ConnectedSlot:
                    return i18n("Connected");
                case Kandas::DisconnectedSlot:
                    return i18n("Disconnected");
                case Kandas::ConnectingSlot:
                    return i18n("Connecting");
                case Kandas::DisconnectingSlot:
                    return i18n("Disconnecting");
            }
        default:
            return QVariant();
    }
}

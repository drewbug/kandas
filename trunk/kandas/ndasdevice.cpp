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

#include "ndasdevice.h"
#include "ndasmodel.h"
#include "ndasslot.h"

#include <QHash>
#include <KIcon>
#include <KIconLoader>
#include <KLocalizedString>

Kandas::Client::NdasDevice::NdasDevice(const QString &name, const QString &serial, Kandas::DeviceState state, bool hasWriteKey)
    : m_name(name)
    , m_serial(serial)
    , m_state(state)
    , m_hasWriteKey(hasWriteKey)
{
}

Kandas::Client::NdasDevice::~NdasDevice()
{
    qDeleteAll(m_slots);
}

void Kandas::Client::NdasDevice::update(const QString &serial, Kandas::DeviceState state, bool hasWriteKey)
{
    m_serial = serial;
    m_state = state;
    m_hasWriteKey = hasWriteKey;
}

QString Kandas::Client::NdasDevice::name() const
{
    return m_name;
}

QString Kandas::Client::NdasDevice::serial() const
{
    return m_serial;
}

Kandas::DeviceState Kandas::Client::NdasDevice::state() const
{
    return m_state;
}

bool Kandas::Client::NdasDevice::hasWriteKey() const
{
    return m_hasWriteKey;
}

QVariant Kandas::Client::NdasDevice::data(int role) const
{
    //some roles that can be served easily
    if (role == Qt::DisplayRole)
        return m_name;
    //gather information - count states
    QHash<Kandas::SlotState, int> stateCounts;
    foreach (Kandas::Client::NdasSlot* slot, m_slots)
        stateCounts[slot->state()] = stateCounts.value(slot->state()) + 1;
    //gather information - fold state counts into target state counts
    QHash<Kandas::SlotState, int> targetStateCounts;
    targetStateCounts[Kandas::SlotOffline] = stateCounts.value(Kandas::SlotOffline);
    targetStateCounts[Kandas::ConnectedSlot] = stateCounts.value(Kandas::ConnectedSlot) + stateCounts.value(Kandas::DisconnectingSlot);
    targetStateCounts[Kandas::DisconnectedSlot] = stateCounts.value(Kandas::DisconnectedSlot) + stateCounts.value(Kandas::ConnectingSlot);
    //look for dominant states
    Kandas::SlotState dominantSlotState;
    if (targetStateCounts[Kandas::ConnectedSlot] + targetStateCounts[Kandas::DisconnectedSlot] == 0)
        dominantSlotState = Kandas::SlotOffline;
    else if (targetStateCounts[Kandas::ConnectedSlot] >= targetStateCounts[Kandas::DisconnectedSlot])
        dominantSlotState = Kandas::ConnectedSlot;
    else
        dominantSlotState = Kandas::DisconnectedSlot;
    bool partial = targetStateCounts[dominantSlotState] == m_slots.count();
    if (partial)
    {
        if (stateCounts.value(Kandas::ConnectingSlot) == m_slots.count())
            dominantSlotState = Kandas::ConnectingSlot;
        else if (stateCounts.value(Kandas::DisconnectingSlot) == m_slots.count())
            dominantSlotState = Kandas::DisconnectingSlot;
    }
    //serve the other roles
    switch (role)
    {
        case Qt::DecorationRole:
            if (dominantSlotState == Kandas::ConnectedSlot || dominantSlotState == Kandas::DisconnectingSlot)
                return KIcon("drive-harddisk", KIconLoader::global(), QStringList() << "emblem-mounted");
            else
                return KIcon("drive-harddisk");
        case Kandas::Client::SecondDecorationRole:
            if (dominantSlotState == Kandas::DisconnectedSlot)
                return KIcon("media-playback-start");
            else if (dominantSlotState == Kandas::ConnectedSlot)
                return KIcon("media-eject");
            else //transitional state or offline
                return KIcon();
        case Kandas::Client::ActionRole:
            if (dominantSlotState == Kandas::DisconnectedSlot)
                return QLatin1String("connect-device");
            else if (dominantSlotState == Kandas::ConnectedSlot)
                return QLatin1String("disconnect-device");
            else //transitional state or offline
                return QString();
        case Kandas::Client::ActionDescriptionRole:
            if (dominantSlotState == Kandas::DisconnectedSlot)
                return i18n("Connect to this drive");
            else if (dominantSlotState == Kandas::ConnectedSlot)
                return i18n("Disconnect from this drive");
            else //transitional state or offline
                return QString();
        case Kandas::Client::SecondDisplayRole:
            switch (m_state)
            {
                case Kandas::DeviceOnline:
                    switch (dominantSlotState)
                    {
                        case Kandas::SlotOffline:
                            return partial ? i18n("Offline") : i18n("Partially offline");
                        case Kandas::ConnectedSlot:
                            return partial ? i18n("Connected") : i18n("Partially connected");
                        case Kandas::ConnectingSlot:
                            return i18n("Connecting");
                        case Kandas::DisconnectedSlot:
                            return partial ? i18n("Disconnected") : i18n("Partially connected");
                        case Kandas::DisconnectingSlot:
                            return i18n("Disconnecting");
                    }
                case Kandas::DeviceOffline:
                    return i18n("Offline");
                case Kandas::DeviceConnectionError:
                    return i18n("Connection error");
                case Kandas::DeviceLoginError:
                    return i18n("Login error");
            }
    }
    return QVariant();
}

void Kandas::Client::NdasDevice::addSlot(Kandas::Client::NdasSlot *slot)
{
    m_slots << slot;
}

void Kandas::Client::NdasDevice::removeSlot(Kandas::Client::NdasSlot *slot)
{
    m_slots.removeAll(slot);
}

QList<Kandas::Client::NdasSlot *> Kandas::Client::NdasDevice::slotList() const
{
    return m_slots;
}

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
    //TODO: placeholder implementation
    if (role == Qt::DisplayRole)
        return i18n("Device %1", m_name);
    else
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

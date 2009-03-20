/***************************************************************************
 *   Copyright (C) 2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "device.h"

#include <QRegExp>
#include <QStringList>

Kandas::Daemon::Device::Device(const QString& metadataLine)
    : m_state(Kandas::DeviceOffline)
{
    static const QRegExp whitespace("\\s+");
    const QStringList metadata = metadataLine.split(whitespace, QString::SkipEmptyParts);
    //read some values from known positions in the line
    m_name = metadata.value(0);
    m_serial = metadata.value(3);
    m_hasWriteKey = metadata.value(2) == QLatin1String("Yes");
    //state may consist of one word in field 5 ("Online", "Offline") or two words in fields 5/6 ("Connect Error", "Login Error")
    if (metadata.value(6) == QLatin1String("Error"))
    {
        if (metadata.value(5) == QLatin1String("Connect"))
            m_state = Kandas::DeviceConnectionError;
        else if (metadata.value(5) == QLatin1String("Login"))
            m_state = Kandas::DeviceLoginError;
    }
    else if (metadata.value(5) == QLatin1String("Online"))
        m_state = Kandas::DeviceOnline;
}

QString Kandas::Daemon::Device::name() const
{
    return m_name;
}

QString Kandas::Daemon::Device::serial() const
{
    return m_serial;
}

Kandas::DeviceState Kandas::Daemon::Device::state() const
{
    return m_state;
}

bool Kandas::Daemon::Device::hasWriteKey() const
{
    return m_hasWriteKey;
}
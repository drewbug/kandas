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

#include "device.h"
#include "engine.h"

#include <QFile>
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

QList<int> Kandas::Daemon::Device::getSlotNumbers() const
{
    QList<int> result;
    //read slot list file
    const QString slotListPath = QString("%1/devices/%2/slots").arg(Kandas::Daemon::Engine::InformationSourceDirectory);
    QFile slotListFile(slotListPath.arg(m_name));
    if (slotListFile.exists() && slotListFile.open(QIODevice::ReadOnly) && slotListFile.isReadable())
    {
        const int bufferSize = 1024; char buffer[bufferSize];
        while (slotListFile.readLine(buffer, bufferSize) != -1)
        {
            const int slotNumber = QString::fromUtf8(buffer).simplified().toInt();
            if (slotNumber == 0) //means that an error occured during the conversion
                continue;
            result << slotNumber;
        }
        slotListFile.close();
    }
    return result;
}

//BEGIN Kandas::Daemon::DeviceList

Kandas::Daemon::DeviceList::DeviceList()
{
}

Kandas::Daemon::DeviceList::DeviceList(const Kandas::Daemon::DeviceList& other)
    : QList<Kandas::Daemon::Device*>(other)
{
}

bool Kandas::Daemon::DeviceList::contains(const QString& deviceName) const
{
    foreach (Kandas::Daemon::Device* device, *this)
        if (device->name() == deviceName)
            return true;
    return false;
}

Kandas::Daemon::Device* Kandas::Daemon::DeviceList::device(const QString& deviceName) const
{
    foreach (Kandas::Daemon::Device* device, *this)
        if (device->name() == deviceName)
            return device;
    return 0;
}

//END Kandas::Daemon::DeviceList
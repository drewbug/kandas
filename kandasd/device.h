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

#ifndef KANDAS_DAEMON_DEVICE_H
#define KANDAS_DAEMON_DEVICE_H

#include "definitions.h"
#include <QList>

namespace Kandas
{
    namespace Daemon
    {

        class Device
        {
            public:
                Device(const QString& metadataLine); //a line from /proc/ndas/devs containing information about the device

                QString name() const;
                QString serial() const;
                Kandas::DeviceState state() const;
                bool hasWriteKey() const;

                QList<int> getSlotNumbers() const; //warning: this method does not provide caching, it reads the slot file on every access
            private:
                QString m_name, m_serial;
                Kandas::DeviceState m_state;
                bool m_hasWriteKey;
        };

        class DeviceList : public QList<Kandas::Daemon::Device*>
        {
            public:
                DeviceList();
                DeviceList(const Kandas::Daemon::DeviceList& other);

                //helper functions (similar to QList::contains and QList::value) that look for devices with a given name
                bool contains(const QString& deviceName) const;
                Kandas::Daemon::Device* device(const QString& deviceName) const;
        };

    }
}

#endif // KANDAS_DAEMON_DEVICE_H

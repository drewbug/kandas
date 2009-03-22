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

#ifndef KANDAS_DAEMON_ENGINE_H
#define KANDAS_DAEMON_ENGINE_H

#include "definitions.h"
#include "device.h"
#include "slot.h"

#include <QHash>
#include <QList>
#include <QTimer>

namespace Kandas
{
    namespace Daemon
    {

        const char DaemonVersionRaw[] = "0.2";
        const QString DaemonVersion(DaemonVersionRaw);
        const char InterfaceVersionRaw[] = "0.2";
        const QString InterfaceVersion(InterfaceVersionRaw);
        const int RefreshInterval = 2000; //refresh every two seconds

        class Device;
        class Slot;

        class Engine : public QObject
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus interface", "org.kandas")
            public:
                Engine();
                bool clean() const;

                static QString InformationSourceDirectory;
            public Q_SLOTS:
                QString daemonVersion();
                QString interfaceVersion();

                void registerClient();
                void unregisterClient();

                int addDevice(const QString &deviceName, const QList<QString> &readKey, const QString &writeKey);
                void removeDevice(const QString &deviceName);

                void connectSlot(int slot, bool readOnly);
                void disconnectSlot(int slot);
                void connectDevice(const QString &device, bool readOnly);
                void disconnectDevice(const QString &device);
            Q_SIGNALS:
                void systemInfo(int state);
                void deviceInfo(const QString &device, const QString &serial, int state, bool hasWriteKey);
                void deviceRemoved(const QString &device);
                void slotInfo(int slot, const QString &device, const QString &blockDevice, int state);
                void slotRemoved(int slot);
                void initComplete();
            private Q_SLOTS:
                void refreshData();
            private:
                bool m_clean;

                int m_clientCount;
                QTimer m_autoRefreshTimer;

                Kandas::SystemState m_system;
                Kandas::Daemon::DeviceList m_devices;
                Kandas::Daemon::SlotList m_slots;
        };

    }
}

#endif // KANDAS_DAEMON_ENGINE_H

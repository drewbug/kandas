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

#ifndef KANDAS_DAEMON_ENGINE_H
#define KANDAS_DAEMON_ENGINE_H

#include "definitions.h"

#include <QHash>
#include <QList>
#include <QTimer>

namespace Kandas
{
    namespace Daemon
    {

        const char DaemonVersionRaw[] = "0.1svn";
        const QString DaemonVersion(DaemonVersionRaw);
        const char InterfaceVersionRaw[] = "0.1";
        const QString InterfaceVersion(InterfaceVersionRaw);

        typedef void (*EngineJob)(int slot);
        struct EngineTask
        {
            EngineJob job;
            int slot;
            EngineTask(EngineJob myJob, int mySlot) : job(myJob), slot(mySlot) {}
        };

        class Engine : public QObject
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus interface", "org.kandas")
            private:
                Engine();
                Engine(const Engine &);
                ~Engine();
            public:
                static Engine *self();
                bool clean() const;
            public Q_SLOTS:
                void registerClient();
                void unregisterClient();
            /*
                void startDriver();
                void stopDriver();
            */
                void connectSlot(int slot, bool readOnly);
                void disconnectSlot(int slot);
                void connectDevice(const QString &device, bool readOnly);
                void disconnectDevice(const QString &device);
                void refreshData();
                void initClient();
                QString daemonVersion();
                QString interfaceVersion();
            Q_SIGNALS:
                void initEnvironmentInfo(int state);
                void initDeviceInfo(const QString &device);
                void initSlotInfo(int slot, const QString &device, int state);
                void initInfoComplete();
                void slotAdded(int slot, const QString &device, int state);
                void slotChanged(int slot, const QString &device, int state);
                void slotRemoved(int slot, const QString &device);
                void deviceAdded(const QString &device);
                void deviceRemoved(const QString &device);
                void environmentChanged(int state);
            private:
            /*
                static void startDriverJob(int);
                static void stopDriverJob(int);
            */
                static void refreshEnvironmentJob(int); //parameter is discarded (needed only for compliance with EngineJob delegate)
                static void refreshDevicesJob(int);
                static void refreshSlotsJob(int);
                static void initClientJob(int);
                static void connectReadJob(int slot);
                static void connectWriteJob(int slot);
                static void disconnectJob(int slot);
            private Q_SLOTS:
                void executeTask();
                void scheduleTask(EngineJob job, int slot = 0);
                void scheduleBlockingTask(EngineJob job, int slot = 0);
            private:
                bool m_clean;

                QString m_infoSourceDir;

                Kandas::EnvironmentState m_envState;
                QList<QString> *m_devices;
                QHash<int, SlotInfo> *m_slots;
                int m_clientCount;

                QList<EngineTask> m_taskQueue;
                QTimer m_taskTimer, m_autoRefreshTimer;
                static const int TaskInterval = 100; //interval between task invocations in milliseconds
        };

    }
}

#endif // KANDAS_DAEMON_ENGINE_H

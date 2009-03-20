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

#ifndef KANDAS_CLIENT_MANAGER_H
#define KANDAS_CLIENT_MANAGER_H

#include "interface.h"

#include <QHash>
#include <QList>
class QModelIndex;
#include <QObject>
#include <kandasd/definitions.h>

namespace Kandas
{
    namespace Client
    {

        class DeviceModel;
        class SlotModel;

        struct SlotInfo
        {
            SlotInfo(int slotNumber, Kandas::SlotState slotState) : slot(slotNumber), state(slotState) {}
            SlotInfo(const SlotInfo &other) : slot(other.slot), state(other.state) {}
            int slot;
            Kandas::SlotState state;
        };

        struct DeviceInfo
        {
            DeviceInfo(const QString &deviceName) : device(deviceName) {}
            DeviceInfo(const DeviceInfo &other) : device(other.device), slotList(other.slotList) {}
            QString device;
            QList<SlotInfo> slotList;
        };

        enum Role { //role extensions for QAbstractItemModel::data
            ConnectionStatusRole = Qt::UserRole + 1,
            ItemIdentifierRole
        };

        class Manager : public QObject
        {
            friend class DeviceModel;
            friend class SlotModel;
            Q_OBJECT
            public:
                Manager();
                ~Manager();

                bool error() const;
                QVariant errorContent(int role) const; //returns QString() if no problem has been found

                DeviceModel *deviceModel() const;
                SlotModel *slotModel() const;
            public slots:
                void connectDevice(const QString &device, bool readOnly);
                void connectSlot(int slot, bool readOnly);
                void disconnectDevice(const QString &device);
                void disconnectSlot(int slot);

                void selectedDeviceChanged(const QModelIndex &device);
                void resetDeviceSelection();
            signals:
                void initializationComplete(const QString &daemonVersion);
            private slots:
                void changeSystem(int systemState);
                void changeDevice(const QString &device, const QString &serial, int state, bool hasWriteKey);
                void changeSlot(int slot, const QString &device, const QString &blockDevice, int state);
                void removeDevice(const QString &device);
                void removeSlot(int slot);
                void initComplete();
            private:
                DeviceModel *m_deviceModel;
                SlotModel *m_slotModel;

                OrgKandasInterface m_interface;
                bool m_connectionClean;

                Kandas::SystemState m_system;
                QList<DeviceInfo> m_devices;
        };

    }
}

#endif //KANDAS_CLIENT_MANAGER_H

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

#include <QObject>
#include <kandasd/definitions.h>

namespace Kandas
{
    namespace Client
    {

        class NdasModel;
        class NdasSystemModel;

        class Manager : public QObject
        {
            Q_OBJECT
            public:
                Manager();
                ~Manager();

                Kandas::Client::NdasModel *model() const;
                Kandas::Client::NdasSystemModel *systemModel() const;
            public Q_SLOTS:
                Kandas::AddDeviceResult addDevice(const QString &device, const QStringList &readKey, const QString &writeKey = QString());

                void connectDevice(const QString &device, bool readOnly);
                void connectSlot(int slot, bool readOnly);
                void disconnectDevice(const QString &device);
                void disconnectSlot(int slot);
            Q_SIGNALS:
                void initializationComplete(const QString &daemonVersion);
                void systemStateChanged(Kandas::SystemState state);
            private Q_SLOTS:
                void changeSystem(int systemState);
                void changeDevice(const QString &device, const QString &serial, int state, bool hasWriteKey);
                void changeSlot(int slot, const QString &device, const QString &blockDevice, int state);
                void removeDevice(const QString &device);
                void removeSlot(int slot);
                void initComplete();
            private:
                OrgKandasInterface m_interface;
                Kandas::Client::NdasModel *m_model;
                Kandas::Client::NdasSystemModel *m_systemModel;
        };

    }
}

#endif //KANDAS_CLIENT_MANAGER_H

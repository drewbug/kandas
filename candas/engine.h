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

#ifndef CANDAS_ENGINE_H
#define CANDAS_ENGINE_H

#include <kandasd/definitions.h>
#include <QHash>
#include <QList>

class OrgKdeKandasInterface;

namespace Kandas
{
    namespace Console
    {

        class Engine : public QObject
        {
            Q_OBJECT
            public:
                Engine(const QList<QString> &upTargets, const QList<QString> &downTargets, const QList<QString> &stateTargets, bool readOnly = false);
                ~Engine();
            private slots:
                void initEnvironment(int state);
                void initDevice(const QString &device);
                void initSlot(int slot, const QString &device, int state);
                void executeJobs();
            private:
                void stateDevice(const QString &device);
                void stateSlot(int slot);
                void upDevice(const QString &device);
                void upSlot(int slot);
                void downDevice(const QString &device);
                void downSlot(int slot);

                QList<QString> m_upTargets, m_downTargets, m_stateTargets; //stores the targets until they get resolved into jobs in Candas::Engine::executeJobs
                bool m_readOnly;
                QHash<int, Kandas::SlotState> m_awaitedStateChanges; //key = slot
                OrgKdeKandasInterface *m_interface;

                Kandas::EnvironmentState m_environment;
                QList<QString> m_devices;
                QHash<int, Kandas::SlotInfo> m_slots;
        };

    }
}

#endif

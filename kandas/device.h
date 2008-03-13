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

#ifndef KANDAS_CLIENT_DEVICE_H
#define KANDAS_CLIENT_DEVICE_H

#include <QGroupBox>

namespace Kandas
{
    namespace Client
    {

        class DevicePrivate;

        class Device : public QGroupBox
        {
            Q_OBJECT
            public:
                Device(const QString &name, QWidget *parent = 0);
                ~Device();
            public slots:
                void addSlot(int slot, int state);
                bool changeSlot(int slot, int state);
                bool removeSlot(int slot);
            private:
                DevicePrivate *p;
        };

    }
}

#endif // KANDAS_CLIENT_DEVICE_H

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

#include "devicemodel.h"
#include "manager.h"

#include <KIcon>
#include <KLocalizedString>

Kandas::Client::DeviceModel::DeviceModel(Kandas::Client::Manager *parent)
    : p(parent)
{
}

Kandas::Client::DeviceModel::~DeviceModel()
{
}

void Kandas::Client::DeviceModel::lateInit()
{
    disconnect(&p->m_interface, SIGNAL(initInfoComplete()), this, SLOT(lateInit()));
    reset();
}

QVariant Kandas::Client::DeviceModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= p->m_devices.count())
        return QVariant();
    int connectedCount = 0;
    int slotCount = 0;
    switch (role)
    {
        case Qt::DisplayRole:
            return p->m_devices.at(row).device;
        case Qt::DecorationRole:
            return KIcon("drive-harddisk");
        case Kandas::Client::ConnectionStatusRole:
            //information about connection status
            foreach (Kandas::Client::SlotInfo info, p->m_devices.at(row).slotList)
            {
                ++slotCount;
                if (info.state == Kandas::Connected)
                    ++connectedCount;
            }
            if (connectedCount == 0)
                return i18n("Not connected");
            else if (connectedCount == slotCount)
                return i18np("%1 slot connected", "%1 slots connected", slotCount);
            else
                return i18n("%1 of %2 slots connected", connectedCount, slotCount);
        default:
            return QVariant();
    }
}

QVariant Kandas::Client::DeviceModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int Kandas::Client::DeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : p->m_devices.count();
}

#include "devicemodel.moc"

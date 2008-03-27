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

#include "slotmodel.h"
#include "manager.h"

#include <KIcon>
#include <KLocalizedString>

//TODO: How does slot list behave if indexes in device list change (e.g. because of a device being added)?

Kandas::Client::SlotModel::SlotModel(Kandas::Client::Manager *parent)
    : p(parent)
    , m_currentDevice(-1)
{
}

Kandas::Client::SlotModel::~SlotModel()
{
}

void Kandas::Client::SlotModel::changeDevice(int index)
{
    if (m_currentDevice != index)
    {
        m_currentDevice = index;
        reset();
    }
}

QVariant Kandas::Client::SlotModel::data(const QModelIndex &index, int role) const
{
    if (m_currentDevice < 0 || m_currentDevice >= p->m_devices.count())
        return QVariant();
    int row = index.row();
    if (row < 0 || row >= p->m_devices.at(m_currentDevice).slotList.count())
        return QVariant();
    Kandas::Client::SlotInfo info(p->m_devices.at(m_currentDevice).slotList.at(row));
    switch (role)
    {
        case Qt::DisplayRole:
            return i18n("Slot %1", info.slot);
        case Qt::DecorationRole:
            return KIcon("unknown");
        case Kandas::Client::ConnectionStatusRole:
            return QString("Hello world");
        default:
            return QVariant();
    }
}

QVariant Kandas::Client::SlotModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int Kandas::Client::SlotModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_currentDevice < 0 || m_currentDevice >= p->m_devices.count())
        return 0;
    return p->m_devices.at(m_currentDevice).slotList.count();
}

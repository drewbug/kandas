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

#include "ndasmodel.h"
#include "ndasdevice.h"
#include "ndasslot.h"

Kandas::Client::NdasModel::NdasModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

Kandas::Client::NdasModel::~NdasModel()
{
    qDeleteAll(m_devices);
}

int Kandas::Client::NdasModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int Kandas::Client::NdasModel::rowCount(const QModelIndex &parent) const
{
    //root level = device level
    if (!parent.isValid())
        return m_devices.count();
    //slot level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(parent.internalPointer());
    if (device)
        return device->slotList().count();
    //no structure below slot level
    return 0;
}

QVariant Kandas::Client::NdasModel::data(const QModelIndex &index, int role) const
{
    //dataItem may be a Kandas::Client::NdasDevice or Kandas::Client::NdasSlot instance
    Kandas::Client::NdasData* dataItem = ndasdata_cast<Kandas::Client::NdasData *>(index.internalPointer());
    if (dataItem)
        return dataItem->data(role);
    else
        return QVariant();
}

QModelIndex Kandas::Client::NdasModel::index(int row, int column, const QModelIndex &parent) const
{
    //structure on X axis
    if (column != 0)
        return QModelIndex();
    //root level = device level
    if (!parent.isValid())
    {
        if (m_devices.value(row) == 0) //bounds checking!
            return QModelIndex();
        else
            return createIndex(row, 0, (void*) m_devices[row]);
    }
    //slot level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(parent.internalPointer());
    if (device)
    {
        QList<Kandas::Client::NdasSlot*> slotList = device->slotList();
        if (slotList.value(row) == 0) //bounds checking!
            return QModelIndex();
        else
            return createIndex(row, 0, (void*) slotList[row]);
    }
    //no structure below slot level
    else
        return QModelIndex();
}

QModelIndex Kandas::Client::NdasModel::parent(const QModelIndex &index) const
{
    //root level = device level
    Kandas::Client::NdasDevice* device = ndasdata_cast<Kandas::Client::NdasDevice*>(index.internalPointer());
    if (device)
        return QModelIndex();
    //slot level -> find device for slot
    Kandas::Client::NdasSlot* slot = ndasdata_cast<Kandas::Client::NdasSlot*>(index.internalPointer());
    if (slot)
    {
        for (int row = 0; row < m_devices.count(); ++row)
            if (m_devices[row]->name() == slot->deviceName())
                return createIndex(row, 0, (void*) m_devices[row]);
    }
    return QModelIndex();
}

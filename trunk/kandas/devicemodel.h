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

#ifndef KANDAS_CLIENT_DEVICEMODEL_H
#define KANDAS_CLIENT_DEVICEMODEL_H

#include <QAbstractListModel>

namespace Kandas
{
    namespace Client
    {
        class Manager;

        class DeviceModel : public QAbstractListModel
        {
            Q_OBJECT
            public:
                DeviceModel(Manager *parent = 0);
                ~DeviceModel();

                QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
                QVariant headerData(int index, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
                int rowCount(const QModelIndex &parent = QModelIndex()) const;
            public slots:
                void lateInit();
            private:
                Manager *p;
        };

    }
}

#endif // KANDAS_CLIENT_DEVICEMODEL_H

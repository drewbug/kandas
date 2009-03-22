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

#ifndef KANDAS_CLIENT_REMOVEDIALOG_H
#define KANDAS_CLIENT_REMOVEDIALOG_H

class QListView;
#include <KDialog>

namespace Kandas
{
    namespace Client
    {

        class Manager;

        class RemoveDialog : public KDialog
        {
            Q_OBJECT
            public:
                RemoveDialog(Kandas::Client::Manager *manager);
            public Q_SLOTS:
                void showDialog();
            private Q_SLOTS:
                void handleInput();
                void handleOkClicked();
            private:
                Kandas::Client::Manager *m_manager;
                QListView *m_view;
        };

    }
}

#endif // KANDAS_CLIENT_REMOVEDIALOG_H

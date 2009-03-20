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

#ifndef KANDAS_CLIENT_WINDOW_H
#define KANDAS_CLIENT_WINDOW_H

#include <KXmlGuiWindow>

namespace Kandas
{
    namespace Client
    {

        class View;

        class MainWindow : public KXmlGuiWindow
        {
            Q_OBJECT
            public:
                explicit MainWindow();
                ~MainWindow();
            public slots:
                void initializationComplete(const QString &daemonVersion);
            private:
                void setupActions();
                View *m_view;
        };

    }
}

#endif // KANDAS_CLIENT_WINDOW_H

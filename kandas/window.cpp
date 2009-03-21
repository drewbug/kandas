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

#include "window.h"
#include "view.h"

#include <KAction>
#include <KActionCollection>
#include <KActionMenu>
#include <KApplication>
#include <KLocalizedString>
#include <KStatusBar>

Kandas::Client::MainWindow::MainWindow()
    : KXmlGuiWindow()
    , m_view(new Kandas::Client::View(this))
{
    //early GUI initialisation
    setupActions();
    statusBar()->insertPermanentItem(i18n("Connecting to KaNDASd..."), 1, 1);
    statusBar()->setItemAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
    setAutoSaveSettings();
    //central widget
    setCentralWidget(m_view);
    connect(m_view, SIGNAL(initializationComplete(const QString &)), this, SLOT(initializationComplete(const QString &)));
    //late GUI initialisation
    setupGUI(QSize(400, 300));
    setWindowIcon(KIcon("folder-remote"));
}

Kandas::Client::MainWindow::~MainWindow()
{
}

void Kandas::Client::MainWindow::setupActions()
{
}

void Kandas::Client::MainWindow::initializationComplete(const QString &daemonVersion)
{
    if (daemonVersion.isEmpty())
        statusBar()->changeItem(i18n("No connection to KaNDASd"), 1);
    else
        statusBar()->changeItem(i18n("Connected to KaNDASd %1", daemonVersion), 1);
    show();
}

#include "window.moc"
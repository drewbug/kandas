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

#include "window.h"
#include "view.h"

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KLocalizedString>
#include <KStatusBar>

Kandas::Client::MainWindow::MainWindow()
    : KXmlGuiWindow()
{
    setupActions();
    setupGUI();
    setWindowIcon(KIcon("folder-remote"));
    slotStateChanged("nodriver");
    statusBar()->hide();
    //central widget
    m_view = new Kandas::Client::View(this);
    setCentralWidget(m_view);
}

Kandas::Client::MainWindow::~MainWindow()
{
}

void Kandas::Client::MainWindow::setupActions()
{
//     KAction *action = actionCollection()->addAction("kandas_start", Kandas::Client::Adaptor::self(), SLOT(ndasDriverStart()));
//     action->setIcon(KIcon("system-suspend-hibernate"));
//     action->setText(i18n("Start driver"));
//     action = actionCollection()->addAction("kandas_stop", Kandas::Client::Adaptor::self(), SLOT(ndasDriverStop()));
//     action->setIcon(KIcon("system-shutdown"));
//     action->setText(i18n("Stop driver"));
//     action = actionCollection()->addAction("kandas_connect_read", Kandas::Client::Adaptor::self(), SLOT(ndasConnectRead()));
//     action->setIcon(KIcon("media-playback-start"));
//     action->setText(i18n("Connect (read)"));
//     action = actionCollection()->addAction("kandas_connect_write", Kandas::Client::Adaptor::self(), SLOT(ndasConnectWrite()));
//     action->setIcon(KIcon("media-record"));
//     action->setText(i18n("Connect (write)"));
//     action = actionCollection()->addAction("kandas_disconnect", Kandas::Client::Adaptor::self(), SLOT(ndasDisconnect()));
//     action->setIcon(KIcon("media-playback-stop"));
//     action->setText(i18n("Disconnect"));
//     action = actionCollection()->addAction("kandas_refresh", Kandas::Client::Adaptor::self(), SLOT(ndasRefresh()));
//     action->setIcon(KIcon("system-switch-user"));
//     action->setText(i18n("Refresh view"));
}

#include "window.moc"

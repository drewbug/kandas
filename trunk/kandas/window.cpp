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
#include "adddialog.h"
#include "removedialog.h"
#include "view.h"

#include <QTimer>
#include <KAction>
#include <KActionCollection>
#include <KActionMenu>
#include <KApplication>
#include <KLocalizedString>
#include <KStatusBar>

Kandas::Client::MainWindow::MainWindow()
    : m_addDialogAct(0)
    , m_removeDialogAct(0)
    , m_addDialog(0)
    , m_removeDialog(0)
    , m_view(new Kandas::Client::View(this))
{
    //early GUI initialisation
    setupActions();
    statusBar()->hide();
    setAutoSaveSettings();
    //central widget
    setCentralWidget(m_view);
    connect(m_view, SIGNAL(initializationComplete(const QString &)), this, SLOT(initializationComplete(const QString &)));
    //late GUI initialisation
    setupGUI();
    setMinimumSize(QSize(300, 300));
    setWindowIcon(KIcon("folder-remote"));
    QTimer::singleShot(0, this, SLOT(setupDialogs()));
}

Kandas::Client::MainWindow::~MainWindow()
{
    delete m_addDialog;
    delete m_removeDialog;
}

void Kandas::Client::MainWindow::setupActions()
{
    m_addDialogAct = new KAction(KIcon("list-add"), i18n("Register drive..."), actionCollection());
    actionCollection()->addAction("kandas_device_add", m_addDialogAct);
    m_removeDialogAct = new KAction(KIcon("list-remove"), i18n("Remove drive..."), actionCollection());
    actionCollection()->addAction("kandas_device_remove", m_removeDialogAct);
}

void Kandas::Client::MainWindow::setupDialogs()
{
    if (!m_addDialog)
    {
        m_addDialog = new Kandas::Client::AddDialog(m_view->manager());
        connect(m_addDialogAct, SIGNAL(triggered()), m_addDialog, SLOT(showDialog()));
    }
    if (!m_removeDialog)
    {
        m_removeDialog = new Kandas::Client::RemoveDialog(m_view->manager());
        connect(m_removeDialogAct, SIGNAL(triggered()), m_removeDialog, SLOT(showDialog()));
    }
}

void Kandas::Client::MainWindow::initializationComplete(const QString &daemonVersion)
{
    bool hasDaemon = !daemonVersion.isEmpty();
    m_addDialogAct->setEnabled(hasDaemon);
    m_removeDialogAct->setEnabled(hasDaemon);
    show();
}

#include "window.moc"

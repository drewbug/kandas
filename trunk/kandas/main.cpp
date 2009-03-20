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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

static const char description[] = I18N_NOOP("KDE adapter for NDAS devices");
static const char version[] = "trunk";

int main(int argc, char ** argv)
{
    KAboutData about("kandas", 0, ki18nc("The application's name", "KaNDAS"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2008 Stefan Majewsky"), KLocalizedString(), "http://code.google.com/p/kandas/", "majewsky@gmx.net");
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about);

    KApplication app;
    new Kandas::Client::MainWindow;
    return app.exec();
}

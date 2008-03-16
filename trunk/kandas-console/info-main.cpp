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

#include "info-worker.h"

#include <iostream>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocalizedString>

static const char description[] = I18N_NOOP("Command line management for NDAS devices");
static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("kandas-info", "kandas", ki18nc("The application's name", "KaNDAS-info"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2008 Stefan Majewsky"));
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("e");
    options.add("environment", ki18n("Show environment state"));
    options.add("d");
    options.add("devices", ki18n("List all available devices"));
    options.add("s");
    options.add("slots", ki18n("List all available slots"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    //read command line options
    bool listEnv = args->isSet("environment");
    bool listDevices = args->isSet("devices");
    bool listSlots = args->isSet("slots");
    args->clear();

    if (!listEnv && !listDevices && !listSlots)
        //when called without arguments, display both lists
        listEnv = listDevices = listSlots = true;

    Kandas::Console::InfoWorker worker(listEnv, listDevices, listSlots);
    if (worker.clean())
        return app.exec();
    else
        return 1;
}

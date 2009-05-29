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

#include "info-worker.h"

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

int main(int argc, char **argv)
{
    KAboutData about("kandas-info", "kandas", ki18n("KaNDAS-info"), Kandas::Console::VersionRaw, ki18n(Kandas::Console::Description), KAboutData::License_GPL, ki18n("Copyright 2008-2009 Stefan Majewsky"), KLocalizedString(), "http://code.google.com/p/kandas/", "majewsky@gmx.net");
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("e");
    options.add("environment", ki18n("Show information about environment state"));
    options.add("d");
    options.add("drives", ki18n("List available drives"));
    options.add("s");
    options.add("slots", ki18n("List available slots"));
    options.add("", ki18n("By default, everything is printed out."));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    //read command line options
    bool listEnv = args->isSet("environment");
    bool listDevices = args->isSet("drives");
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

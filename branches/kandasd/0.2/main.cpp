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

#include "engine.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KCmdLineOptions>
#include <KProcess>

static const char description[] = I18N_NOOP("KDE adapter daemon for NDAS devices");

int main(int argc, char ** argv)
{
    KAboutData about("kandasd", "kandas", ki18nc("The application's name", "KaNDASd"), Kandas::Daemon::DaemonVersionRaw, ki18n(description), KAboutData::License_GPL, ki18n("Copyright 2008-2009 Stefan Majewsky"), KLocalizedString(), "http://code.google.com/p/kandas/", "majewsky@gmx.net");
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgNone);

    KCmdLineOptions options;
    options.add("d");
    options.add("detach", ki18n("Start detached instance of KaNDASd (for usage on console)"));
    options.add("s");
    options.add("source <directory>", ki18n("NDAS source directory (use this with KaNDASsimul)"), "/proc/ndas");
    options.add("", ki18n("KaNDASd has to run with root privileges."));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app(false);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("detach"))
    {
        KProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList() << "--source" << args->getOption("source"));
        return 0;
    }
    Kandas::Daemon::Engine::InformationSourceDirectory = args->getOption("source");
    args->clear();

    Kandas::Daemon::Engine engine;
    if (engine.clean())
        return app.exec();
    else
        return 1;
}

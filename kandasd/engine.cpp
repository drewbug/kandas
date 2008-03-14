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

#include "engine.h"
#include "kandasadaptor.h"

#include <QDir>
#include <QFile>
#include <KDebug> //do not remove, is needed by kError and kWarning
#include <KProcess>

//TODO: What is the difference between a slot and a device?

//singleton functions

Kandas::Daemon::Engine::Engine()
    : QObject()
    , m_clean(true)
    , m_envState(Kandas::UnknownEnvironment)
    , m_devices(new QList<QString>())
    , m_slots(new QHash<int, Kandas::SlotInfo>())
    , m_clientCount(new QHash<Kandas::ClientType, int>)
{
    //task timer
    connect(&m_taskTimer, SIGNAL(timeout()), this, SLOT(executeTask()));
    connect(&m_taskTimer, SIGNAL(timeout()), this, SLOT(refreshData()));
    //D-Bus
    new KandasAdaptor(this);
    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.registerService("org.kde.kandas"))
    {
        kError() << "Could not register service. D-Bus returned:" << bus.lastError().message();
        m_clean = false;
    }
    else
        bus.registerObject("/", this);
    //schedule initial data update
    refreshData();
}

Kandas::Daemon::Engine::~Engine()
{
}

Kandas::Daemon::Engine::Engine(const Engine&)
    : QObject()
{
}

Kandas::Daemon::Engine *Kandas::Daemon::Engine::self()
{
    static Kandas::Daemon::Engine *engine = new Kandas::Daemon::Engine;
    return engine;
}

bool Kandas::Daemon::Engine::clean()
{
    return m_clean;
}

//slots

void Kandas::Daemon::Engine::registerClient(int intType)
{
    Kandas::ClientType type;
    switch (intType)
    {
        case Kandas::HighImportanceMonitor:
            type = Kandas::HighImportanceMonitor;
            break;
        case Kandas::LowImportanceMonitor:
            type = Kandas::LowImportanceMonitor;
            break;
        case Kandas::DirectModifier:
            type = Kandas::DirectModifier;
            break;
        default:
            return;
    }
    QHash<Kandas::ClientType, int> *clientCount = self()->m_clientCount;
    (*clientCount)[type] = clientCount->value(type, 0) + 1;
    //determine new value for auto-refresh timing
    if (clientCount->value(Kandas::HighImportanceMonitor, 0) > 0)
        self()->m_autoRefreshTimer.start(Kandas::AutoRefreshIntervals::HighImportanceMonitor);
    else if (clientCount->value(Kandas::LowImportanceMonitor, 0) > 0)
        self()->m_autoRefreshTimer.start(Kandas::AutoRefreshIntervals::LowImportanceMonitor);
    else
    {
        self()->m_autoRefreshTimer.stop();
        //update once if a direct modifier has been registered
        if (type == Kandas::DirectModifier)
            self()->refreshData();
    }
}

void Kandas::Daemon::Engine::unregisterClient(int intType)
{
    Kandas::ClientType type;
    switch (intType)
    {
        case Kandas::HighImportanceMonitor:
            type = Kandas::HighImportanceMonitor;
            break;
        case Kandas::LowImportanceMonitor:
            type = Kandas::LowImportanceMonitor;
            break;
        case Kandas::DirectModifier:
            type = Kandas::DirectModifier;
            break;
        default:
            return;
    }
    QHash<Kandas::ClientType, int> *clientCount = self()->m_clientCount;
    (*clientCount)[type] = qMax(0, clientCount->value(type, 0) - 1);
    //determine new value for auto-refresh timing
    if (clientCount->value(Kandas::HighImportanceMonitor, 0) > 0)
        self()->m_autoRefreshTimer.start(Kandas::AutoRefreshIntervals::HighImportanceMonitor);
    else if (clientCount->value(Kandas::LowImportanceMonitor, 0) > 0)
        self()->m_autoRefreshTimer.start(Kandas::AutoRefreshIntervals::LowImportanceMonitor);
    else
        self()->m_autoRefreshTimer.stop();
}

void Kandas::Daemon::Engine::startDriver()
{
    scheduleTask(&Kandas::Daemon::Engine::startDriverJob);
}

void Kandas::Daemon::Engine::stopDriver()
{
    scheduleTask(&Kandas::Daemon::Engine::stopDriverJob);
}

void Kandas::Daemon::Engine::connectSlot(int slot, bool readonly)
{
    scheduleTask(readonly ? &Kandas::Daemon::Engine::connectReadJob : &Kandas::Daemon::Engine::connectWriteJob, slot);
}

void Kandas::Daemon::Engine::disconnectSlot(int slot)
{
    scheduleTask(&Kandas::Daemon::Engine::disconnectJob, slot);
}

void Kandas::Daemon::Engine::connectDevice(const QString &device, bool readonly)
{
    QHashIterator<int, Kandas::SlotInfo> iterSlots(*(self()->m_slots));
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        if (iterSlots.value().device == device)
            scheduleTask(readonly ? &Kandas::Daemon::Engine::connectReadJob : &Kandas::Daemon::Engine::connectWriteJob, iterSlots.key());
    }
}

void Kandas::Daemon::Engine::disconnectDevice(const QString &device)
{
    QHashIterator<int, Kandas::SlotInfo> iterSlots(*(self()->m_slots));
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        if (iterSlots.value().device == device)
            scheduleTask(&Kandas::Daemon::Engine::disconnectJob, iterSlots.key());
    }
}

void Kandas::Daemon::Engine::refreshData()
{
    scheduleTask(&Kandas::Daemon::Engine::refreshEnvironmentJob);
    scheduleTask(&Kandas::Daemon::Engine::refreshDevicesJob);
    scheduleTask(&Kandas::Daemon::Engine::refreshSlotsJob);
}

void Kandas::Daemon::Engine::initClient()
{
    scheduleBlockingTask(&Kandas::Daemon::Engine::initClientJob);
}

QString Kandas::Daemon::Engine::engineVersion()
{
    return Kandas::Daemon::Version;
}

//task engine

void Kandas::Daemon::Engine::executeTask()
{
    EngineTask task = m_taskQueue.takeFirst();
    (*(task.job))(task.slot);
    if (m_taskQueue.count() == 0)
        m_taskTimer.stop();
}

void Kandas::Daemon::Engine::scheduleTask(Kandas::Daemon::EngineJob job, int slot)
{
    if (m_taskQueue.count() == 0 && !m_taskTimer.isActive())
        m_taskTimer.start(Kandas::Daemon::Engine::TaskInterval);
    Kandas::Daemon::EngineTask newTask;
    newTask.job = job;
    newTask.slot = slot;
    m_taskQueue << newTask;
}

void Kandas::Daemon::Engine::scheduleBlockingTask(Kandas::Daemon::EngineJob job, int slot)
{
    if (m_taskQueue.count() == 0 && !m_taskTimer.isActive())
        m_taskTimer.start(Kandas::Daemon::Engine::TaskInterval);
    Kandas::Daemon::EngineTask newTask;
    newTask.job = job;
    newTask.slot = slot;
    m_taskQueue.prepend(newTask);
}

//queueable jobs

void Kandas::Daemon::Engine::refreshEnvironmentJob(int)
{
    Kandas::EnvironmentState newState = Kandas::UnknownEnvironment;
    //check availability of NDAS driver
    QDir dir(Kandas::Daemon::runtimeInfoDir);
    if (!dir.exists())
        newState = Kandas::NoDriverFound;
    //check availability of ndasadmin
    if (newState == Kandas::UnknownEnvironment)
    {
        KProcess process;
        QStringList args; args << "ndasadmin";
        process.setProgram(QString("which"), args);
        process.setOutputChannelMode(KProcess::OnlyStdoutChannel);
        process.start();
        process.waitForFinished();
        if (QString::fromUtf8(process.readAllStandardOutput()).simplified().isEmpty())
            newState = Kandas::NoAdminFound;
    }
    //environment is sane if no regression has been found
    if (newState == Kandas::UnknownEnvironment)
        newState = Kandas::SaneEnvironment;
    //propagate environment state change if necessary
    if (newState != self()->m_envState)
        emit self()->environmentChanged(newState);
    self()->m_envState = newState;
}

void Kandas::Daemon::Engine::refreshDevicesJob(int)
{
    //if environment is not sane, clear devices list
    if (self()->m_envState != Kandas::SaneEnvironment)
    {
        self()->m_devices->clear();
        foreach (QString device, *(self()->m_devices))
            emit self()->deviceRemoved(device);
        return;
    }
    //save copy of old device list (necessary for signals)
    QList<QString> oldDevices(*(self()->m_devices));
    //clear devices list
    self()->m_devices->clear();
    //open devices list
    QFile file(QString("%1/devs").arg(Kandas::Daemon::runtimeInfoDir));
    if (file.exists() && file.open(QIODevice::ReadOnly) && file.isReadable())
    {
        char buffer[1024];
        file.readLine(buffer, sizeof(buffer)); //skip first line (human-readable captions)
        static const QRegExp whitespace("\\s+");
        while (file.readLine(buffer, sizeof(buffer)) != -1)
            *(self()->m_devices) << QString::fromUtf8(buffer).simplified().section(whitespace, 0, 0);
        file.close();
    }
    else //devices list could not be read
        self()->scheduleTask(&Kandas::Daemon::Engine::refreshEnvironmentJob);
    //send update signals
    foreach (QString device, oldDevices)
        if (!self()->m_devices->contains(device))
            emit self()->deviceRemoved(device);
    foreach (QString device, *(self()->m_devices))
        if (!oldDevices.contains(device))
            emit self()->deviceAdded(device);
}

void Kandas::Daemon::Engine::refreshSlotsJob(int)
{
    //if environment is not sane, clear slot list
    if (self()->m_envState != Kandas::SaneEnvironment)
    {
        QMutableHashIterator<int, Kandas::SlotInfo> iterSlots(*(self()->m_slots));
        while (iterSlots.hasNext())
        {
            iterSlots.next();
            emit self()->slotRemoved(iterSlots.key(), iterSlots.value().device);
            iterSlots.remove();
        }
        return;
    }
    //save copy of old slot list (necessary for signals)
    const QHash<int, Kandas::SlotInfo> oldSlots(*(self()->m_slots));
    //clear slot list
    self()->m_slots->clear();
    //read slot numbers from device lists
    char buffer[1024];
    foreach (QString device, *(self()->m_devices))
    {
        QFile file(QString("%2/devices/%1/slots").arg(device).arg(Kandas::Daemon::runtimeInfoDir));
        Kandas::SlotInfo slotInfo(device);
        if (file.exists() && file.open(QIODevice::ReadOnly) && file.isReadable())
        {
            while(file.readLine(buffer, sizeof(buffer)) != -1)
            {
                int slot = QString::fromUtf8(buffer).simplified().toInt();
                if (slot != 0) //a return value of 0 from toInt() means that an error occured during the conversion
                    (*(self()->m_slots))[slot] = slotInfo;
            }
            file.close();
        }
    }
    //slot state
    QMutableHashIterator<int, Kandas::SlotInfo> iterSlots(*(self()->m_slots));
    while (iterSlots.hasNext())
    {
        iterSlots.next();
        int slot = iterSlots.key();
        Kandas::SlotInfo &info = iterSlots.value();
        //read slot info file
        QFile file(QString("%2/slots/%1/info").arg(slot).arg(Kandas::Daemon::runtimeInfoDir));
        if (!file.exists() || !file.open(QIODevice::ReadOnly) || !file.isReadable())
            continue;
        file.readLine(buffer, sizeof(buffer)); //skip first line (human-readable captions)
        if (file.readLine(buffer, sizeof(buffer)) == -1) //second line should contain slot info
            continue;
        const QString line = QString::fromUtf8(buffer).simplified();
        file.close();
        //is slot enabled?
        info.state = (line.section(' ', 0, 0) == "Enabled") ? Kandas::Connected : Kandas::Disconnected;
        //transitional states
        const Kandas::SlotState oldState = oldSlots[slot].state;
        if (oldState == Kandas::Connecting && info.state == Kandas::Disconnected
        ||  oldState == Kandas::Disconnecting && info.state == Kandas::Connected)
            info.state = oldState;
    }
    //propagate updates
    QHashIterator<int, Kandas::SlotInfo> iterOld(oldSlots);
    while (iterOld.hasNext())
    {
        iterOld.next();
        int slot = iterOld.key();
        if (self()->m_slots->contains(slot))
        {
            //TODO: More fine-grained: If device has changed, emit slotRemoved and slotAdded. If state has changed to 0, emit slotRemoved. If state has not changed, do not emit anything.
            const Kandas::SlotInfo info = (*(self()->m_slots))[slot], oldInfo = iterOld.value();
            if (info.device != oldInfo.device)
            {
                emit self()->slotRemoved(slot, oldInfo.device);
                emit self()->slotAdded(slot, info.device, info.state);
            }
            else if (info.state == 0)
            {
                self()->m_slots->remove(slot);
                emit self()->slotRemoved(slot, oldInfo.device);
            }
            else if (info.state != oldInfo.state)
                emit self()->slotChanged(slot, info.device, info.state);
        }
        else
            emit self()->slotRemoved(slot, iterOld.value().device);
    }
    QHashIterator<int, Kandas::SlotInfo> iterNew(*(self()->m_slots));
    while (iterNew.hasNext())
    {
        iterNew.next();
        int slot = iterNew.key();
        if (!oldSlots.contains(slot))
        {
            Kandas::SlotInfo info = iterNew.value();
            emit self()->slotAdded(slot, info.device, info.state);
        }
    }
}

void Kandas::Daemon::Engine::startDriverJob(int)
{
    //check state
    Kandas::EnvironmentState envState = self()->m_envState;
    if (envState == Kandas::UnknownEnvironment || envState == Kandas::SaneEnvironment)
        return;
    //call process
    KProcess process;
    QStringList args;
    args << QString("start");
    process.setProgram("ndasadmin", args);
    process.start();
    process.waitForFinished(20000);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshEnvironmentJob);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshDevicesJob);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshSlotsJob);
}

void Kandas::Daemon::Engine::stopDriverJob(int)
{
    //check state
    if (self()->m_envState != Kandas::SaneEnvironment)
        return;
    bool driverHasToWait = false, waitingForSlotInfo = false;
    QHashIterator<int, Kandas::SlotInfo> i(*(self()->m_slots));
    while (i.hasNext())
    {
        i.next();
        Kandas::SlotState state = i.value().state;
        if (state == Kandas::Disconnected)
            continue;
        //incorrect state - perform necessary actions, then retry
        driverHasToWait = true;
        if (state == Kandas::Undetermined)
        {
            if (!waitingForSlotInfo) //only schedule slot update once
            {
                self()->scheduleBlockingTask(&Kandas::Daemon::Engine::refreshSlotsJob);
                waitingForSlotInfo = true;
            }
        }
        else if (state == Kandas::Connected)
            self()->scheduleTask(&Kandas::Daemon::Engine::disconnectJob, i.key());
        else
            kWarning() << QString("State %1 of slot %2 has not been handled correctly.").arg(state).arg(i.key());
        //retry after necessary actions have been performed
        self()->scheduleTask(&Kandas::Daemon::Engine::stopDriverJob);
    }
    //call process
    KProcess process;
    QStringList args;
    args << QString("stop");
    process.setProgram("ndasadmin", args);
    process.start();
    process.waitForFinished(20000);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshEnvironmentJob);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshDevicesJob);
    self()->scheduleTask(&Kandas::Daemon::Engine::refreshSlotsJob);
}

void Kandas::Daemon::Engine::initClientJob(int)
{
    //environment
    emit self()->initEnvironmentInfo(self()->m_envState);
    //devices
    foreach (QString device, *(self()->m_devices))
        emit self()->initDeviceInfo(device);
    //slots
    QHashIterator<int, Kandas::SlotInfo> iterSlots(*(self()->m_slots));
    while (iterSlots.hasNext())
    {
        Kandas::SlotInfo info = iterSlots.next().value();
        emit self()->initSlotInfo(iterSlots.key(), info.device, info.state);
    }
    emit self()->initInfoComplete();
}

void Kandas::Daemon::Engine::connectReadJob(int /*slot*/)
{
    //TODO: Kandas::Daemon::Engine::connectReadJob
}

void Kandas::Daemon::Engine::connectWriteJob(int /*slot*/)
{
    //TODO: Kandas::Daemon::Engine::connectWriteJob
}

void Kandas::Daemon::Engine::waitForConnectedJob(int /*slot*/)
{
    //TODO: Kandas::Daemon::Engine::waitForConnectedJob
}

void Kandas::Daemon::Engine::disconnectJob(int /*slot*/)
{
    //TODO: Kandas::Daemon::Engine::disconnectJob
}

void Kandas::Daemon::Engine::waitForDisconnectedJob(int /*slot*/)
{
    //TODO: Kandas::Daemon::Engine::waitForDisconnectedJob
}

#include "engine.moc"

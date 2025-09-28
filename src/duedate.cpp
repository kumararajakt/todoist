#include "duedate.h"

#include <QChronoTimer>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

DueDate::DueDate(QObject *parent)
    : QObject(parent)
{
}

DueDate::~DueDate()
{
    // remove all running timer
}

void DueDate::addTaskDueDate(int taskIndex, const QDateTime &dueDate)
{
    // TODO: check if task is already completed
    m_dueDates[taskIndex] = dueDate;

    qint64 msecsTo = QDateTime::currentDateTime().msecsTo(dueDate);
    QChronoTimer *timer = new QChronoTimer(std::chrono::milliseconds(msecsTo), this);
    connect(timer, &QChronoTimer::timeout, this, [taskIndex, this]() {
        Q_EMIT remindTrigger(taskIndex);
    });
}

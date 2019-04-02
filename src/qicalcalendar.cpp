#include "qicalcalendar.h"

QiCalCalendar::QiCalCalendar(QObject *parent) : QObject(parent)
{

}

QString QiCalCalendar::prodId() const
{
    return m_prodId;
}

void QiCalCalendar::setProdId(const QString &prodId)
{
    m_prodId = prodId;
    emit prodIdChanged();
}

QString QiCalCalendar::version() const
{
    return m_version;
}

void QiCalCalendar::setVersion(const QString &version)
{
    m_version = version;
    emit versionChanged();
}

QString QiCalCalendar::method() const
{
    return m_method;
}

void QiCalCalendar::setMethod(const QString &method)
{
    m_method = method;
    emit methodChanged();
}

QList<QiCalTimeZone *> QiCalCalendar::timeZones() const
{
    return m_timeZones;
}

void QiCalCalendar::addTimeZone(QiCalTimeZone *timeZone)
{
    timeZone->setParent(this);
    m_timeZones.push_back(timeZone);
    emit timeZonesChanged();
}

void QiCalCalendar::removeTimeZone(QiCalTimeZone *timeZone)
{
    m_timeZones.removeOne(timeZone);
    delete timeZone;
    emit timeZonesChanged();
}

void QiCalCalendar::clearTimeZones()
{
    for (QiCalTimeZone* zone : m_timeZones)
    {
        delete zone;
    }

    m_timeZones.clear();
    emit timeZonesChanged();
}

const QList<QiCalEvent *> &QiCalCalendar::events() const
{
    return m_events;
}

void QiCalCalendar::addEvent(QiCalEvent *event)
{
    event->setParent(this);
    m_events.push_back(event);
    emit eventsChanged();
}

void QiCalCalendar::removeEvent(QiCalEvent *event)
{
    m_events.removeOne(event);
    delete event;
    emit eventsChanged();
}

void QiCalCalendar::clearEvents()
{
    for (QiCalEvent* event : m_events)
    {
        delete event;
    }

    m_events.clear();
    emit eventsChanged();
}

QList<QiCalRule *> QiCalCalendar::rules() const
{
    return m_rules;
}

void QiCalCalendar::setRules(const QList<QiCalRule *> &rules)
{
    m_rules = rules;
}

void QiCalCalendar::addRule(QiCalRule *rule)
{
    m_rules.push_back(rule);
}

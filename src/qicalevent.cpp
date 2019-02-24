#include "qicalevent.h"

QiCalEvent::QiCalEvent(QObject *parent) : QObject(parent),
    m_status(STAT_TENTATIVE),
    m_transp(TRANS_OPAQUE)
{
}

QDateTime QiCalEvent::dtStart() const
{
    return m_dtStart;
}

void QiCalEvent::setDtStart(const QDateTime &dtStart)
{
    m_dtStart = dtStart;
    emit dtStartChanged();;
}

QDateTime QiCalEvent::dtEnd() const
{
    return m_dtEnd;
}

void QiCalEvent::setDtEnd(const QDateTime &dtEnd)
{
    m_dtEnd = dtEnd;
    emit dtEndChanged();
}

QDateTime QiCalEvent::dtStamp() const
{
    return m_dtStamp;
}

void QiCalEvent::setDtStamp(const QDateTime &dtStamp)
{
    m_dtStamp = dtStamp;
    emit dtStampChanged();
}

QString QiCalEvent::uid() const
{
    return m_uid;
}

void QiCalEvent::setUid(const QString &uid)
{
    m_uid = uid;
    emit uidChanged();
}

QDateTime QiCalEvent::created() const
{
    return m_created;
}

void QiCalEvent::setCreated(const QDateTime &created)
{
    m_created = created;
    emit createdChanged();
}

QString QiCalEvent::description() const
{
    return m_description;
}

void QiCalEvent::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QDateTime QiCalEvent::lastModified() const
{
    return m_lastModified;
}

void QiCalEvent::setLastModified(const QDateTime &lastModified)
{
    m_lastModified = lastModified;
    emit lastModifiedChanged();
}

QString QiCalEvent::location() const
{
    return m_location;
}

void QiCalEvent::setLocation(const QString &location)
{
    m_location = location;
    emit locationChanged();
}

QiCalEvent::Status QiCalEvent::status() const
{
    return m_status;
}

void QiCalEvent::setStatus(const Status &status)
{
    m_status = status;
    emit statusChanged();
}

QiCalEvent::Transp QiCalEvent::transp() const
{
    return m_transp;
}

void QiCalEvent::setTransp(const Transp &transp)
{
    m_transp = transp;
    emit transpChanged();
}

QList<QiCalAlarm *> QiCalEvent::alarms() const
{
    return m_alarms;
}

void QiCalEvent::addAlarm(QiCalAlarm *alarm)
{
    alarm->setParent(this);
    m_alarms.push_back(alarm);
    emit alarmsChanged();
}

void QiCalEvent::removeAlarm(QiCalAlarm *alarm)
{
    m_alarms.removeOne(alarm);
    delete alarm;
    emit alarmsChanged();
}

void QiCalEvent::clearAlarms()
{
    for (QiCalAlarm* alarm : m_alarms)
    {
        delete alarm;
    }

    m_alarms.clear();
    emit alarmsChanged();
}

QString QiCalEvent::summary() const
{
    return m_summary;
}

void QiCalEvent::setSummary(const QString &summary)
{
    m_summary = summary;
    emit summaryChanged();
}

bool QiCalEvent::operator <(const QiCalEvent &other) const
{
    return dtStart() < other.dtStart();
}

QiCalRule *QiCalEvent::rule() const
{
    return m_rule;
}

void QiCalEvent::setRule(QiCalRule *rule)
{
    rule->setParent(this);
    m_rule = rule;
    emit ruleChanged();
}

QiCalAlarm::QiCalAlarm(QObject *parent) : QObject(parent),
    m_action(ACT_AUDIO)
{
}

QiCalAlarm::Action QiCalAlarm::action() const
{
    return m_action;
}

void QiCalAlarm::setAction(const Action &action)
{
    m_action = action;
    emit actionChanged();
}

QString QiCalAlarm::description() const
{
    return m_description;
}

void QiCalAlarm::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QString QiCalAlarm::trigger() const
{
    return m_trigger;
}

void QiCalAlarm::setTrigger(const QString &trigger)
{
    m_trigger = trigger;
    emit triggerChanged();
}

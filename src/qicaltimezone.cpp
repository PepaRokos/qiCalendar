#include "qicaltimezone.h"

QiCalTimeZone::QiCalTimeZone(QObject *parent) : QObject(parent),
    m_standard(nullptr),
    m_dayLight(nullptr)
{
}

QString QiCalTimeZone::tzId() const
{
    return m_tzId;
}

void QiCalTimeZone::setTzId(const QString &tzId)
{
    m_tzId = tzId;
    emit tzIdChanged();
}

QiCalTzInfo *QiCalTimeZone::standard() const
{
    return m_standard;
}

void QiCalTimeZone::setStandard(QiCalTzInfo *standard)
{
    standard->setParent(this);
    m_standard = standard;
    emit standardChanged();
}

QiCalTzInfo *QiCalTimeZone::dayLight() const
{
    return m_dayLight;
}

void QiCalTimeZone::setDayLight(QiCalTzInfo *dayLight)
{
    dayLight->setParent(this);
    m_dayLight = dayLight;
    emit dayLightChanged();
}

QiCalTzInfo::QiCalTzInfo(QObject *parent) : QObject(parent),
    m_offsetFrom(0),
    m_offsetTo(0)
{
}

int QiCalTzInfo::offsetFrom() const
{
    return m_offsetFrom;
}

void QiCalTzInfo::setOffsetFrom(int offsetFrom)
{
    m_offsetFrom = offsetFrom;
    emit offsetFromChanged();
}

int QiCalTzInfo::offsetTo() const
{
    return m_offsetTo;
}

void QiCalTzInfo::setOffsetTo(int offsetTo)
{
    m_offsetTo = offsetTo;
    emit offsetToChanged();
}

QString QiCalTzInfo::tzName() const
{
    return m_tzName;
}

void QiCalTzInfo::setTzName(const QString &tzName)
{
    m_tzName = tzName;
    emit tzNameChanged();
}

QDateTime QiCalTzInfo::dtStart() const
{
    return m_dtStart;
}

void QiCalTzInfo::setDtStart(const QDateTime &dtStart)
{
    m_dtStart = dtStart;
    emit dtStartChanged();
}

QiCalRule *QiCalTzInfo::rule() const
{
    return m_rule;
}

void QiCalTzInfo::setRule(QiCalRule *rule)
{
    rule->setParent(this);
    m_rule = rule;
    emit ruleChanged();
}

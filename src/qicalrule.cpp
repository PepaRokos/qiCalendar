#include "qicalrule.h"

QiCalRule::QiCalRule(QObject *parent) : QObject(parent),
    m_count(-1),
    m_interval(1),
    m_event(nullptr)
{
}

QiCalRule::Freq QiCalRule::freq() const
{
    return m_freq;
}

void QiCalRule::setFreq(const Freq &freq)
{
    m_freq = freq;
    emit freqChanged();
}

QDateTime QiCalRule::until() const
{
    return m_until;
}

void QiCalRule::setUntil(const QDateTime &until)
{
    m_until = until;
    emit untilChanged();
}

qint32 QiCalRule::count() const
{
    return m_count;
}

void QiCalRule::setCount(const qint32 &count)
{
    m_count = count;
    emit countChanged();
}

qint32 QiCalRule::interval() const
{
    return m_interval;
}

void QiCalRule::setInterval(const qint32 &interval)
{
    m_interval = interval;
    emit intervalChanged();
}

QList<qint32> QiCalRule::bySecond() const
{
    return m_bySecond;
}

void QiCalRule::setBySecond(const QList<qint32> &bySecond)
{
    m_bySecond = bySecond;
    emit bySecondChanged();
}

void QiCalRule::setSecondList(const QString &seconds)
{
    fillIntList(seconds, m_bySecond);
    emit bySecondChanged();
}

QString QiCalRule::secondList() const
{
    return getIntList(m_bySecond);
}

QList<qint32> QiCalRule::byMinute() const
{
    return m_byMinute;
}

void QiCalRule::setByMinute(const QList<qint32> &byMinute)
{
    m_byMinute = byMinute;
    emit byMinuteChanged();
}

void QiCalRule::setMinuteList(const QString &minute)
{
    fillIntList(minute, m_byMinute);
    emit byMinuteChanged();
}

QString QiCalRule::minuteList() const
{
    return getIntList(m_byMinute);
}

QList<qint32> QiCalRule::byHour() const
{
    return m_byHour;
}

void QiCalRule::setByHour(const QList<qint32> &byHour)
{
    m_byHour = byHour;
    emit byHourChanged();
}

void QiCalRule::setHourList(const QString &hour)
{
    fillIntList(hour, m_byHour);
    emit byHourChanged();
}

QString QiCalRule::hourList() const
{
    return getIntList(m_byHour);
}

QList<QString> QiCalRule::byDay() const
{
    return m_byDay;
}

void QiCalRule::setByDay(const QList<QString> &byDay)
{
    m_byDay = byDay;
    emit byDayChanged();
}

void QiCalRule::setDayList(const QString &day)
{
    m_byDay = day.split(",");
    emit byDayChanged();
}

QString QiCalRule::dayList() const
{
    return getStringList(m_byDay);
}

QList<qint32> QiCalRule::byMonthDay() const
{
    return m_byMonthDay;
}

void QiCalRule::setByMonthDay(const QList<qint32> &byMonthDay)
{
    m_byMonthDay = byMonthDay;
    emit byMonthDayChanged();
}

void QiCalRule::setMonthDayList(const QString &month)
{
    fillIntList(month, m_byMonthDay);
    emit byMonthDayChanged();
}

QString QiCalRule::monthDayList() const
{
    return getIntList(m_byMonthDay);
}

QList<qint32> QiCalRule::byYearDay() const
{
    return m_byYearDay;
}

void QiCalRule::setByYearDay(const QList<qint32> &byYearDay)
{
    m_byYearDay = byYearDay;
    emit byYearDayChanged();
}

void QiCalRule::setYearDayList(const QString &year)
{
    fillIntList(year, m_byYearDay);
    emit byYearDayChanged();
}

QString QiCalRule::yearDayList() const
{
    return getIntList(m_byYearDay);
}

QList<qint32> QiCalRule::byWeekNo() const
{
    return m_byWeekNo;
}

void QiCalRule::setByWeekNo(const QList<qint32> &byWeekNo)
{
    m_byWeekNo = byWeekNo;
    emit byWeekNoChanged();
}

void QiCalRule::setWeekList(const QString &week)
{
    fillIntList(week, m_byWeekNo);
    emit byWeekNoChanged();
}

QString QiCalRule::weekList() const
{
    return getIntList(m_byWeekNo);
}

QList<qint32> QiCalRule::byMonth() const
{
    return m_byMonth;
}

void QiCalRule::setByMonth(const QList<qint32> &byMonth)
{
    m_byMonth = byMonth;
    emit byMonthChanged();
}

void QiCalRule::setMonthList(const QString &month)
{
    fillIntList(month, m_byMonth);
    emit byMonthChanged();
}

QString QiCalRule::monthList() const
{
    return getIntList(m_byMonth);
}

QList<qint32> QiCalRule::bySetPos() const
{
    return m_bySetPos;
}

void QiCalRule::setBySetPos(const QList<qint32> &bySetPos)
{
    m_bySetPos = bySetPos;
    emit bySetPosChanged();
}

void QiCalRule::setSetposList(const QString &pos)
{
    fillIntList(pos, m_bySetPos);
    emit bySetPosChanged();
}

QString QiCalRule::setposList() const
{
    return getIntList(m_bySetPos);
}

QString QiCalRule::wkst() const
{
    return m_wkst;
}

void QiCalRule::setWkst(const QString &Wkst)
{
    m_wkst = Wkst;
    emit wkstChanged();
}

QiCalEvent *QiCalRule::calEvent() const
{
    return m_event;
}

void QiCalRule::setCalEvent(QiCalEvent *event)
{
    m_event = event;
    emit calEventChanged();
}

void QiCalRule::fillIntList(const QString &strList, QList<qint32> &list)
{
    list.clear();
    QStringList sList = strList.split(",");
    for (QString str : sList)
    {
        list.push_back(str.toInt());
    }
}

QString QiCalRule::getIntList(const QList<qint32> &list) const
{
    QString ret;
    for (qint32 val : list)
    {
        if (!ret.isEmpty())
        {
            ret += ",";
        }

        ret += QString::number(val);
    }

    return ret;
}

QString QiCalRule::getStringList(const QList<QString> &list) const
{
    QString ret;
    for (QString str : list)
    {
        if (!ret.isEmpty())
        {
            ret += ",";
        }

        ret += str;
    }

    return ret;
}

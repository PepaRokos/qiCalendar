#include "qicalendar.h"

#include <QVariant>
#include <QString>
#include <QFile>
#include <QDebug>
#include <algorithm>
#include <QtAlgorithms>
#include <thread>
#include <future>
#include <tuple>

using namespace std::placeholders;

QiCalendarParser::QiCalendarParser() :
    m_calendar(nullptr)
{
    m_states = {
        {CAL_ROOT, {
             {"VCALENDAR", [&](){
                  m_calendar = new QiCalCalendar();
                  m_state.push(QiCalendarParser::CAL_CALENDAR);}}
         }
        },
        {CAL_CALENDAR, {
             {"VTIMEZONE", [&](){
                  m_calendar->addTimeZone(new QiCalTimeZone());
                  m_state.push(QiCalendarParser::CAL_TIMEZONE);}},
             {"VEVENT", [&](){
                  m_calendar->addEvent(new QiCalEvent());
                  m_state.push(QiCalendarParser::CAL_EVENT);}}
         }
        },
        {CAL_TIMEZONE, {
             {"STANDARD", [&](){
                  m_calendar->timeZones().last()->setStandard(new QiCalTzInfo());
                  m_state.push(QiCalendarParser::CAL_TZINFO_STD);}},
             {"DAYLIGHT", [&](){
                  m_calendar->timeZones().last()->setDayLight(new QiCalTzInfo());
                  m_state.push(QiCalendarParser::CAL_TZINFO_DAYLIGHT);}}
         }
        },
        {CAL_EVENT, {
             {"VALARM", [&](){
                  m_calendar->events().last()->addAlarm(new QiCalAlarm());
                  m_state.push(QiCalendarParser::CAL_ALARM);}}
         }
        },
    };

    m_stateMap = {
        {"VCALENDAR", CAL_CALENDAR},
        {"VTIMEZONE", CAL_TIMEZONE},
        {"STANDARD", CAL_TZINFO_STD},
        {"DAYLIGHT", CAL_TZINFO_DAYLIGHT},
        {"VEVENT", CAL_EVENT},
        {"VALARM", CAL_ALARM}
    };

    m_keyWords = {
        { CAL_ROOT, {
              {"BEGIN", VCAL_BEGIN},
              {"END", VCAL_END}
          }
        },
        { CAL_CALENDAR , {
              {"BEGIN", VCAL_BEGIN},
              {"PRODID", VCAL_STRING("prodId")},
              {"VERSION", VCAL_STRING("version")},
              {"METHOD", VCAL_STRING("method")},
              {"END", VCAL_END}
          }
        },
        { CAL_TIMEZONE, {
              {"BEGIN", VCAL_BEGIN},
              {"TZID", VCAL_STRING("tzId")},
              {"END", VCAL_END}
          }
        },
        { CAL_TZINFO_STD, {
              {"TZOFFSETFROM", VCAL_INTEGER("offsetFrom")},
              {"TZOFFSETTO", VCAL_INTEGER("offsetTo")},
              {"TZNAME", VCAL_STRING("tzName")},
              {"DTSTART", VCAL_DATETIME("dtStart")},
              {"RRULE", VCAL_TZRULE},
              {"END", VCAL_END}
          }
        },
        { CAL_TZINFO_DAYLIGHT, {
              {"TZOFFSETFROM", VCAL_INTEGER("offsetFrom")},
              {"TZOFFSETTO", VCAL_INTEGER("offsetTo")},
              {"TZNAME", VCAL_STRING("tzName")},
              {"DTSTART", VCAL_DATETIME("dtStart")},
              {"RRULE", VCAL_TZRULE},
              {"END", VCAL_END}
          }
        },
        { CAL_EVENT, {
              {"BEGIN", VCAL_BEGIN},
              {"DTSTART", VCAL_DATETIME("dtStart")},
              {"DTEND", VCAL_DATETIME("dtEnd")},
              {"DTSTAMP", VCAL_DATETIME("dtStamp")},
              {"UID", VCAL_STRING("uid")},
              {"CREATED", VCAL_DATETIME("created")},
              {"DESCRIPTION", VCAL_STRING("description")},
              {"SUMMARY", VCAL_STRING("summary")},
              {"LAST-MODIFIED", VCAL_DATETIME("lastModified")},
              {"STATUS", VCAL_EVTSTATUS},
              {"TRANSP", VCAL_EVTTRANSP},
              {"RRULE", VCAL_TZRULE},
              {"END", VCAL_END}
          }

        },
        { CAL_ALARM, {
              {"DESCRIPTION", VCAL_STRING("description")},
              {"ACTION", VCAL_ALARMACTION},
              {"TRIGGER", VCAL_STRING("trigger")},
              {"END", VCAL_END}
          }
        }
    };

    m_rRules = {
        { "FREQ", [&](const QString& value){
              QHash<QString, QiCalRule::Freq> freqs = {
                  {"SECONDLY", QiCalRule::RR_SECONDLY},
                  {"MINUTELY", QiCalRule::RR_MINUTELY},
                  {"HOURLY", QiCalRule::RR_HOURLY},
                  {"DAILY", QiCalRule::RR_DAILY},
                  {"WEEKLY", QiCalRule::RR_WEEKLY},
                  {"MONTHLY", QiCalRule::RR_MONTHLY},
                  {"YEARLY", QiCalRule::RR_YEARLY}
              };

              setObjectValue("freq", freqs[value]);
          }
        },
        { "BYMONTH", VCAL_INTEGER("monthList")},
        { "BYDAY", VCAL_STRING("dayList")},
        { "BYHOUR", VCAL_STRING("hourList")},
        { "BYMINUTE", VCAL_STRING("minuteList")},
        { "BYMONTHDAY", VCAL_STRING("monthDayList")},
        { "BYSECOND", VCAL_STRING("secondList")},
        { "BYSETPOS", VCAL_STRING("setposList")},
        { "WKST", VCAL_STRING("wkst")},
        { "INTERVAL", VCAL_INTEGER("interval")},
        { "COUNT", VCAL_INTEGER("count")},
        { "UNTIL", VCAL_DATETIME("until")}
    };

    m_alActions = {
        { "AUDIO", QiCalAlarm::ACT_AUDIO },
        { "DISPLAY", QiCalAlarm::ACT_DISPLAY },
        { "EMAIL", QiCalAlarm::ACT_EMAIL }
    };

    m_evtStatuses = {
        { "TENTATIVE", QiCalEvent::STAT_TENTATIVE },
        { "CONFIRMED", QiCalEvent::STAT_CONFIRMED },
        { "CANCELLED", QiCalEvent::STAT_CANCELLED }
    };

    m_evtTransps = {
        { "OPAQUE", QiCalEvent::TRANS_OPAQUE },
        { "TRANSPARENT", QiCalEvent::TRANS_TRANSPARENT }
    };

    m_weekDays = {
        { Qt::Monday, "MO" },
        { Qt::Tuesday, "TU" },
        { Qt::Wednesday, "WE" },
        { Qt::Thursday, "TH" },
        { Qt::Friday, "FR" },
        { Qt::Saturday, "SA" },
        { Qt::Sunday, "SU" }
    };

    m_wkst = {
        { "SA", {"SA", "SU", "MO", "TU", "WE", "TH", "FR"} },
        { "SU", {"SU", "MO", "TU", "WE", "TH", "FR", "SA"} },
        { "MO", {"MO", "TU", "WE", "TH", "FR", "SA", "SU"} }
    };
}

bool QiCalendarParser::parseFile(const QString &filePath)
{
    if (m_calendar)
    {
        delete m_calendar;
        m_calendar = nullptr;
    }

    m_state.clear();
    m_state.push(CAL_ROOT);

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    QByteArray lineData = file.readLine();
    while (!lineData.isEmpty())
    {
        QString line(lineData);
        QStringList cmdVal = line.split(":");

        if (cmdVal.size() > 1)
        {
            QString cmd = cmdVal[0];
            if (!m_keyWords[m_state.top()][cmd] && cmd.startsWith("DT"))
            {
                QStringList dtCmd = cmd.split(";");
                if (dtCmd.size() > 1)
                {
                    cmd = dtCmd[0];
                }
            }

            if (m_keyWords[m_state.top()][cmd])
            {
                QString value = cmdVal[1].trimmed();
                m_keyWords[m_state.top()][cmd](value);
            }
        }

        lineData = file.readLine();
    }

    return m_calendar != nullptr;
}

QList<QiCalEvent *> QiCalendarParser::eventsFrom(const QDateTime &from)
{
    QList<QiCalEvent*> ret;
    for (QiCalEvent* ev : m_calendar->events())
    {
        if (ev->dtStart() >= from)
        {
            ret.push_back(ev);
        }
    }

    return ret;
}

QList<QiCalEvent *> QiCalendarParser::eventsRange(const QDateTime &from, const QDateTime &to)
{
    //auto ruleEvents = std::async(std::launch::async, [&](){ return genRuleEvents(from, to); });

    QList<QiCalEvent*> ret;
    for (QiCalEvent* ev : m_calendar->events())
    {
        if (ev->dtStart() >= from && ev->dtStart() <= to)
        {
            ret.push_back(ev);
        }
    }

    ret.append(genRuleEvents(from, to));
    std::sort(ret.begin(), ret.end(), [](QiCalEvent* eventA, QiCalEvent* eventB) {
        return eventA->dtStart() < eventB->dtStart();
    });

    return ret;
}

void QiCalendarParser::parseString(const QString &propertyName, const QString &value)
{
    setObjectValue(propertyName, value);
}

void QiCalendarParser::parseInt(const QString &propertyName, const QString &value)
{
    int val = value.toInt();
    setObjectValue(propertyName, val);
}

void QiCalendarParser::parseDate(const QString &propertyName, const QString &value)
{
    QDateTime date = QDateTime::fromString(value, "yyyyMMddThhmmss");

    if (!date.isValid())
    {
        date = QDateTime::fromString(value, "yyyyMMddThhmmssZ");
    }

    if (!date.isValid())
    {
        date = QDateTime::fromString(value, "yyyyMMdd");
    }

    if (!date.isValid())
    {
        date = QDateTime::fromString("01011970T000000", "ddMMyyyyThhmmss");
    }

    setObjectValue(propertyName, date);
}

void QiCalendarParser::parseRule(const QString &value)
{
    QiCalRule* rule = new QiCalRule();
    if (!currentObject()->setProperty("rule", QVariant::fromValue(rule)))
    {
        delete rule;
        return;
    }

    QiCalEvent* evt = qobject_cast<QiCalEvent*>(currentObject());

    if (evt != nullptr)
    {
        rule->setCalEvent(evt);
        m_calendar->addRule(rule);
    }

    m_state.push(CAL_RRULE);

    QStringList params = value.split(";");

    for (QString param : params)
    {
        QStringList values = param.split("=");
        if (values.count() == 2 && m_rRules[values[0]])
        {
            m_rRules[values[0]](values[1]);
        }
    }

    m_state.pop();
}

void QiCalendarParser::parseAlarmAction(const QString &value)
{
    setObjectValue("action", m_alActions[value]);
}

void QiCalendarParser::parseEvtStatus(const QString &value)
{
    setObjectValue("status", m_evtStatuses[value]);
}

void QiCalendarParser::parseEvtTransp(const QString &value)
{
    setObjectValue("transp", m_evtTransps[value]);
}

void QiCalendarParser::setObjectValue(const QString &propertyName, const QVariant &value)
{
    QObject* cur = currentObject();

    if (cur)
    {
        cur->setProperty(propertyName.toStdString().c_str(), value);
    }
}

void QiCalendarParser::switchState(const QString &state)
{
    auto switchFn = m_states[m_state.top()][state];

    if (switchFn)
    {
        switchFn();
    }
}

void QiCalendarParser::endState(const QString &state)
{
    if (m_stateMap[state] == m_state.top())
    {
        m_state.pop();
    }
}

QObject *QiCalendarParser::currentObject()
{
    auto stateObject = [&](State state) -> QObject* {
        switch (state) {
        case CAL_CALENDAR:
            return m_calendar;
        case CAL_TIMEZONE:
            return m_calendar->timeZones().last();
        case CAL_TZINFO_STD:
            return m_calendar->timeZones().last()->standard();
        case CAL_TZINFO_DAYLIGHT:
            return m_calendar->timeZones().last()->dayLight();
        case CAL_EVENT:
            return m_calendar->events().last();
        case CAL_ALARM:
            return m_calendar->events().last()->alarms().last();
        default:
            return nullptr;
        }
    };

    if (m_state.top() != CAL_RRULE)
    {
        return stateObject(m_state.top());
    }

    QVariant var = stateObject(m_state[m_state.count() - 2])->property("rule");

    if (!var.isNull())
    {
        return qvariant_cast<QObject*>(var);
    }

    return nullptr;
}

QList<QiCalEvent *> QiCalendarParser::genRuleEvents(const QDateTime &from, const QDateTime &to)
{
    QList<QiCalEvent*> result;

    const auto addEvent = [&](QiCalRule* rule, const QDateTime& current) {
        QiCalEvent* event = new QiCalEvent(m_calendar);
        event->setCreated(rule->calEvent()->created());
        event->setDescription(rule->calEvent()->description());
        if (rule->calEvent()->dtEnd().isValid())
        {
            QDateTime dtEnd = current;
            dtEnd.setTime(rule->calEvent()->dtEnd().time());
            event->setDtEnd(dtEnd.addDays(rule->calEvent()->dtStart().daysTo(rule->calEvent()->dtEnd())));
        }
        event->setDtStart(QDateTime(current.date(), rule->calEvent()->dtStart().time()));
        event->setDtStamp(rule->calEvent()->dtStamp());
        event->setLastModified(rule->calEvent()->lastModified());
        event->setLocation(rule->calEvent()->location());
        event->setStatus(rule->calEvent()->status());
        event->setSummary(rule->calEvent()->summary());
        event->setTransp(rule->calEvent()->transp());
        event->setUid(rule->calEvent()->uid());

        result.push_back(event);
    };

    const auto calcEnd = [&to](QiCalRule* rule, QDateTime&& countEnd) -> QDateTime {
        QDateTime endDate;
        if (rule->count() > -1)
        {
            endDate = countEnd;
        }
        if ((rule->until().isValid() && endDate.isValid() && rule->until() < endDate)
                || (rule->until().isValid() && !endDate.isValid()))
        {
            endDate = rule->until();
        }

        if (!endDate.isValid())
        {
            endDate = to;
        }

        return endDate;
    };

    const auto dayWithOrder = [](const QString& byDay) -> std::tuple<qint8, QString> {
        QString day = byDay.right(2);
        QString num = QString(byDay).replace(day, "");
        qint8 dayOrder = num.toInt();

        return std::make_tuple(dayOrder, day);
    };

    const auto dayList = [&](QiCalRule* rule, const QDate& curDate) -> QList<QDate> {
        QList<QDate> retList;
        for (quint8 day : rule->byMonthDay())
        {
            QDate date(curDate.year(), curDate.month(), day);
            retList << date;
        }

        for (const QString& byDay : rule->byDay())
        {
            QString day;
            qint8 dayOrder = 0;
            std::tie(dayOrder, day) = dayWithOrder(byDay);

            auto it = std::find_if(m_weekDays.begin(), m_weekDays.end(), [&day](const auto& dayName) {
                return dayName == day;
            });

            if (it == m_weekDays.end())
            {
                continue;
            }

            Qt::DayOfWeek dow = (Qt::DayOfWeek)it.key();
            QDate firstDay = QDate(curDate.year(), curDate.month(), 1);
            qint8 firstOffset = dow - firstDay.dayOfWeek();
            firstOffset = firstOffset >= 0 ? firstOffset : 7 + firstOffset;
            QDate lastDay = QDate(curDate.year(), curDate.month() + 1, 1).addDays(-1);
            quint8 lastOffset = lastDay.dayOfWeek() - dow;
            lastOffset = lastOffset > 0 ? lastOffset : 7 + lastOffset;

            if (dayOrder > 0)
            {
                retList << firstDay.addDays(firstOffset + 7 * (dayOrder - 1));
            }
            else if (dayOrder < 0)
            {
                retList << lastDay.addDays(-(lastOffset + 7 * (dayOrder -1)));
            }
            else
            {
                QDate resDate(curDate.year(), curDate.month(), firstOffset);
                while (resDate.month() == curDate.month())
                {
                    retList << resDate;
                    resDate = resDate.addDays(7);
                }
            }
        }

        if (retList.isEmpty())
        {
            QDate date(curDate.year(), curDate.month(), rule->calEvent()->dtStart().date().day());
            retList << date;
        }

        std::sort(retList.begin(), retList.end());

        return retList;
    };

    for (QiCalRule* rule : m_calendar->rules())
    {
        if (rule->calEvent() == nullptr)
        {
            continue;
        }

        QDateTime start = rule->calEvent()->dtStart();
        switch (rule->freq()) {
        case QiCalRule::RR_DAILY:
        {
            QDateTime endDate(calcEnd(rule, start.addDays(rule->count() * rule->interval() - 1)));
            QDateTime current = from;

            while (current <= endDate)
            {
                if (start.daysTo(current) % rule->interval() == 0)
                {
                    addEvent(rule, current);
                    current = current.addDays(rule->interval());
                }
                else
                {
                    current = current.addDays(1);
                }
            }
            break;
        }
        case QiCalRule::RR_WEEKLY:
        {
            QDateTime endDate(calcEnd(rule, start.addDays(rule->count() * 7 * rule->interval() - 7)));
            QDateTime current = from;
            QStringList wkst;
            int dayOffsets[7] = { 0 };

            if (rule->byDay().count() > 1 && !rule->wkst().isEmpty())
            {
                wkst = m_wkst[rule->wkst()];
                QList<quint16> dayNums;

                for (const QString& day : rule->byDay())
                {
                    dayNums << wkst.indexOf(day);
                }

                std::sort(dayNums.begin(), dayNums.end());

                for (int i = 0; i < dayNums.count(); i++)
                {
                    if (i == dayNums.count() - 1)
                    {
                        dayOffsets[dayNums[i]] = 7 - dayNums[i] + 7 * (rule->interval() - 1);
                    }
                    else
                    {
                        dayOffsets[dayNums[i]] = dayNums[i + 1] - dayNums[i];
                    }
                }
            }

            QStringList days = rule->byDay();

            if (days.isEmpty())
            {
                days << m_weekDays[start.date().dayOfWeek()];
            }

            while (current <= endDate)
            {
                if (days.contains(m_weekDays[current.date().dayOfWeek()]) && (start.daysTo(current) / 7) % rule->interval() == 0)
                {
                    addEvent(rule, current);
                    current = current.addDays(wkst.isEmpty() ? 7 * rule->interval() : dayOffsets[wkst.indexOf(m_weekDays[current.date().dayOfWeek()])]);
                }
                else if (days.contains(m_weekDays[current.date().dayOfWeek()]) && (start.daysTo(current) / 7) % rule->interval() != 0)
                {
                    current = current.addDays(wkst.isEmpty() ? 7 : dayOffsets[wkst.indexOf(m_weekDays[current.date().dayOfWeek()])]);
                }
                else
                {
                    current = current.addDays(1);
                }
            }
            break;
        }
        case QiCalRule::RR_MONTHLY:
        {
            QDateTime endDate(calcEnd(rule, start.addMonths(rule->count() * rule->interval() - 1)));
            QDateTime current = from;

            QList<QDate> dates;
            quint8 month = 0;
            while (current <= endDate)
            {
                if ( ((current.date().month() - start.date().month()) % rule->interval() != 0)
                        || (!rule->byMonth().isEmpty() && !rule->byMonth().contains(current.date().month())) )
                {
                    current = current.addMonths(1);
                    continue;
                }

                if (month != current.date().month())
                {
                    dates = dayList(rule, current.date());
                    month = current.date().month();
                }

                if (dates.contains(current.date()))
                {
                    addEvent(rule, current);
                    dates.removeOne(current.date());
                }

                if (dates.isEmpty() && endDate.date().month() < month + 1)
                {
                    break;
                }

                current = current.addDays(1);
            }
            break;
        }
        case QiCalRule::RR_YEARLY:
        {
            QDateTime endDate(calcEnd(rule, start.addYears(rule->count() * rule->interval() - 1)));
            QDateTime current = from;
            quint8 month = 0;
            QList<QDate> dates;

            while (current <= endDate)
            {
                if ((current.date().year() - start.date().year()) % rule->interval() != 0)
                {
                    current = current.addYears(1);
                    continue;
                }

                QList<qint32> weeks = rule->byWeekNo();
                std::sort(weeks.begin(), weeks.end());

                if (month != current.date().month())
                {
                    dates = dayList(rule, current.date());
                    month = current.date().month();
                }

                // by weekno
                if (weeks.contains(current.date().weekNumber()) && dates.contains(current.date()))
                {
                    addEvent(rule, current);

                    if (rule->byDay().count() > 1)
                    {
                        current = current.addDays(1);
                    }
                    else if (weeks.count() > 1 && weeks.indexOf(current.date().weekNumber()) < weeks.count() - 1)
                    {
                        current = current.addDays((weeks[weeks.indexOf(current.date().weekNumber()) + 1] - current.date().weekNumber()) * 7);
                    }
                    else
                    {
                        current = current.addYears(1);
                    }

                    continue;
                }

                QList<qint32> months = rule->byMonth();
                std::sort(months.begin(), months.end());

                // by month
                if (months.contains(current.date().month()) && dates.contains(current.date()))
                {
                    addEvent(rule, current);

                    if (rule->byDay().count() > 1 || rule->byMonthDay().count() > 1)
                    {
                        current = current.addDays(1);
                    }
                    else if (months.count() > 1 && months.indexOf(current.date().month()) < months.count() - 1)
                    {
                        current.setDate(QDate(current.date().year(), current.date().month() + 1, 1));
                    }
                    else
                    {
                        current = current.addYears(1);
                    }

                    continue;
                }

                QDate eventDate = rule->calEvent()->dtStart().date();
                QDate currentEventDate(current.date().year(), eventDate.month(), eventDate.day());

                if (current.date() <= currentEventDate && currentEventDate <= endDate.date())
                {
                    QDateTime eventDateTime = rule->calEvent()->dtStart();
                    eventDateTime.setDate(currentEventDate);
                    addEvent(rule, eventDateTime);
                }

                current = current.addYears(1);
            }

            break;
        }
        default:
            break;
        }
    }

    return result;
}

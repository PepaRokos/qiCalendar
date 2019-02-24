#include "qicalendar.h"

#include <QVariant>
#include <QString>
#include <QFile>
#include <QDebug>
#include <algorithm>
#include <QtAlgorithms>

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

    m_tzRules = {
        { "FREQ", [&](const QString& value){
              QHash<QString, QiCalRule::Freq> freqs = {
                  {"SECONDLY", QiCalRule::TZ_SECONDLY},
                  {"MINUTELY", QiCalRule::TZ_MINUTELY},
                  {"HOURLY", QiCalRule::TZ_HOURLY},
                  {"WEEKLY", QiCalRule::TZ_WEEKLY},
                  {"MONTHLY", QiCalRule::TZ_MONTHLY},
                  {"YEARLY", QiCalRule::TZ_YEARLY}
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
        { "WKST", VCAL_STRING("wkstList")},
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
    QList<QiCalEvent*> ret;
    for (QiCalEvent* ev : m_calendar->events())
    {
        if (ev->dtStart() >= from && ev->dtStart() <= to)
        {
            ret.push_back(ev);
        }
    }

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

void QiCalendarParser::parseTzRule(const QString &value)
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
        if (values.count() == 2 && m_tzRules[values[0]])
        {
            m_tzRules[values[0]](values[1]);
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

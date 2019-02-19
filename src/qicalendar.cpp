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
                  m_calendar->timeZones().last()->setStandard(new QiCalTzInfo());
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
              {"RULE", VCAL_TZRULE},
              {"END", VCAL_END}
          }
        },
        { CAL_TZINFO_DAYLIGHT, {
              {"TZOFFSETFROM", VCAL_INTEGER("offsetFrom")},
              {"TZOFFSETTO", VCAL_INTEGER("offsetTo")},
              {"TZNAME", VCAL_STRING("tzName")},
              {"DTSTART", VCAL_DATETIME("dtStart")},
              {"RULE", VCAL_TZRULE},
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
              {"LOCATION", VCAL_STRING("location")},
              {"END", VCAL_END}
          }

        },
        { CAL_ALARM, {
              {"DESCRIPTION", VCAL_STRING("description")},
              {"END", VCAL_END}
          }
        }
    };

    m_tzRules = {
        { "FREQ", [&](const QString& value){
              QMap<QString, QiCalTzInfo::Freq> freqs = {
                  {"SECONDLY", QiCalTzInfo::TZ_SECONDLY},
                  {"MINUTELY", QiCalTzInfo::TZ_MINUTELY},
                  {"HOURLY", QiCalTzInfo::TZ_HOURLY},
                  {"WEEKLY", QiCalTzInfo::TZ_WEEKLY},
                  {"MONTHLY", QiCalTzInfo::TZ_MONTHLY},
                  {"YEARLY", QiCalTzInfo::TZ_YEARLY}
              };

              setObjectValue("freq", freqs[value]);
          }
        },
        { "BYMONTH", VCAL_INTEGER("monthStart")},
        { "BYDAY", VCAL_STRING("dayStart")}
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

        if (cmdVal.size() > 1 && m_keyWords[m_state.top()][cmdVal[0]])
        {
            QString value = cmdVal[1].trimmed();
            m_keyWords[m_state.top()][cmdVal[0]](value);
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
        date = QDateTime::fromString("01.01.1970T000000", "ddMMyyyyThhmmss");
    }

    setObjectValue(propertyName, date);
}

void QiCalendarParser::parseTzRule(const QString &value)
{
    QStringList params = value.split(";");

    for (QString param : params)
    {
        QStringList values = param.split("=");
        if (values.count() == 2 && m_tzRules[values[0]])
        {
            m_tzRules[values[0]](values[1]);
        }
    }
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

QObject *QiCalendarParser::currentObject()
{
    switch (m_state.top()) {
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
}

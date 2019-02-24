#ifndef QICALENDAR_H
#define QICALENDAR_H

#include <QMap>
#include <QString>
#include <QStack>
#include <QHash>

#include <functional>

#include "qicalcalendar.h"
#include "qicalendar_global.h"

class QICALENDARSHARED_EXPORT QiCalendarParser
{

public:
    QiCalendarParser();

    bool parseFile(const QString& file);
    QiCalCalendar* calendar();
    QList<QiCalEvent*> eventsFrom(const QDateTime& from);
    QList<QiCalEvent*> eventsRange(const QDateTime& from, const QDateTime& to);

private:
    enum State
    {
        CAL_ROOT = 0,
        CAL_CALENDAR,
        CAL_TIMEZONE,
        CAL_TZINFO_STD,
        CAL_TZINFO_DAYLIGHT,
        CAL_EVENT,
        CAL_ALARM,
        CAL_RRULE
    };

    void parseString(const QString& propertyName, const QString& value);
    void parseInt(const QString& propertyName, const QString& value);
    void parseDate(const QString& propertyName, const QString& value);
    void parseTzRule(const QString& value);
    void parseAlarmAction(const QString& value);
    void parseEvtStatus(const QString& value);
    void parseEvtTransp(const QString& value);

    void setObjectValue(const QString& propertyName, const QVariant& value);

    void switchState(const QString& state);
    QObject *currentObject();

    QHash<State, QHash<QString, std::function<void(const QString&)> > > m_keyWords;
    QHash<State, QHash<QString, std::function<void()> > > m_states;
    QHash<QString, std::function<void(const QString&)> > m_tzRules;
    QHash<QString, QiCalAlarm::Action> m_alActions;
    QHash<QString, QiCalEvent::Status> m_evtStatuses;
    QHash<QString, QiCalEvent::Transp> m_evtTransps;
    QStack<State> m_state;

    QiCalCalendar* m_calendar;
};

#define VCAL_BEGIN std::bind(&QiCalendarParser::switchState, this, _1)
#define VCAL_END [&](const QString&){ m_state.pop(); }
#define VCAL_STRING(prop) [&](const QString& val){ parseString(prop, val); }
#define VCAL_INTEGER(prop) [&](const QString& val){ parseInt(prop, val); }
#define VCAL_DATETIME(prop) [&](const QString& val){ parseDate(prop, val); }
#define VCAL_TZRULE [&](const QString& val){ parseTzRule(val); }
#define VCAL_ALARMACTION [&](const QString& val){ parseAlarmAction(val); }
#define VCAL_EVTSTATUS [&](const QString& val){ parseEvtStatus(val); }
#define VCAL_EVTTRANSP [&](const QString& val){ parseEvtTransp(val); }

#endif // QICALENDAR_H

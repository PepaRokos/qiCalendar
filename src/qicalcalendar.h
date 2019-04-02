#ifndef QICALCALENDAR_H
#define QICALCALENDAR_H

#include <QObject>
#include <QString>
#include <QList>

#include "qicaltimezone.h"
#include "qicalevent.h"
#include "qicalrule.h"

class QiCalCalendar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString prodId READ prodId WRITE setProdId NOTIFY prodIdChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QList<QiCalTimeZone*> timeZones READ timeZones NOTIFY timeZonesChanged)
    Q_PROPERTY(QList<QiCalEvent*> events READ events NOTIFY eventsChanged)
public:
    explicit QiCalCalendar(QObject *parent = nullptr);

    QString prodId() const;
    void setProdId(const QString &prodId);

    QString version() const;
    void setVersion(const QString &version);

    QString method() const;
    void setMethod(const QString &method);

    QList<QiCalTimeZone *> timeZones() const;
    void addTimeZone(QiCalTimeZone* timeZone);
    void removeTimeZone(QiCalTimeZone* timeZone);
    void clearTimeZones();

    const QList<QiCalEvent *>& events() const;
    void addEvent(QiCalEvent* event);
    void removeEvent(QiCalEvent* event);
    void clearEvents();

    QList<QiCalRule *> rules() const;
    void setRules(const QList<QiCalRule *> &rules);
    void addRule(QiCalRule* rule);

signals:
    void prodIdChanged();
    void versionChanged();
    void methodChanged();
    void timeZonesChanged();
    void eventsChanged();

private:
    QString m_prodId;
    QString m_version;
    QString m_method;
    QList<QiCalTimeZone*> m_timeZones;
    QList<QiCalEvent*> m_events;
    QList<QiCalRule*> m_rules;
};

#endif // QICALCALENDAR_H

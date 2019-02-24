#ifndef QICALTIMEZONE_H
#define QICALTIMEZONE_H

#include <QObject>
#include <QString>
#include <QDateTime>

#include "qicalrule.h"

class QiCalTzInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int offsetFrom READ offsetFrom WRITE setOffsetFrom NOTIFY offsetFromChanged)
    Q_PROPERTY(int offsetTo READ offsetTo WRITE setOffsetTo NOTIFY offsetToChanged)
    Q_PROPERTY(QString tzName READ tzName WRITE setTzName NOTIFY tzNameChanged)
    Q_PROPERTY(QDateTime dtStart READ dtStart WRITE setDtStart NOTIFY dtStartChanged)
    Q_PROPERTY(QiCalRule* rule READ rule WRITE setRule NOTIFY ruleChanged)
public:
    explicit QiCalTzInfo(QObject *parent = nullptr);

    int offsetFrom() const;
    void setOffsetFrom(int offsetFrom);

    int offsetTo() const;
    void setOffsetTo(int offsetTo);

    QString tzName() const;
    void setTzName(const QString &tzName);

    QDateTime dtStart() const;
    void setDtStart(const QDateTime &dtStart);

    QiCalRule *rule() const;
    void setRule(QiCalRule *rule);

signals:
    void offsetFromChanged();
    void offsetToChanged();
    void tzNameChanged();
    void dtStartChanged();
    void ruleChanged();

private:
    int m_offsetFrom;
    int m_offsetTo;
    QString m_tzName;
    QDateTime m_dtStart;
    QiCalRule* m_rule;
};

class QiCalTimeZone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString tzId READ tzId WRITE setTzId NOTIFY tzIdChanged)
    Q_PROPERTY(QiCalTzInfo* standard READ standard WRITE setStandard NOTIFY standardChanged)
    Q_PROPERTY(QiCalTzInfo* dayLight READ dayLight WRITE setDayLight NOTIFY dayLightChanged)
public:
    explicit QiCalTimeZone(QObject *parent = nullptr);

    QString tzId() const;
    void setTzId(const QString &tzId);

    QiCalTzInfo *standard() const;
    void setStandard(QiCalTzInfo *standard);

    QiCalTzInfo *dayLight() const;
    void setDayLight(QiCalTzInfo *dayLight);

signals:
    void tzIdChanged();
    void standardChanged();
    void dayLightChanged();

private:
    QString m_tzId;
    QiCalTzInfo* m_standard;
    QiCalTzInfo* m_dayLight;
};

#endif // QICALTIMEZONE_H

#ifndef QICALTIMEZONE_H
#define QICALTIMEZONE_H

#include <QObject>
#include <QString>
#include <QDateTime>

class QiCalTzInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int offsetFrom READ offsetFrom WRITE setOffsetFrom NOTIFY offsetFromChanged)
    Q_PROPERTY(int offsetTo READ offsetTo WRITE setOffsetTo NOTIFY offsetToChanged)
    Q_PROPERTY(QString tzName READ tzName WRITE setTzName NOTIFY tzNameChanged)
    Q_PROPERTY(QDateTime dtStart READ dtStart WRITE setDtStart NOTIFY dtStartChanged)
    Q_PROPERTY(Freq freq READ freq WRITE setFreq NOTIFY freqChanged)
    Q_PROPERTY(int monthStart READ monthStart WRITE setMonthStart NOTIFY monthStartChanged)
    Q_PROPERTY(QString dayStart READ dayStart WRITE setDayStart NOTIFY dayStartChanged)
public:
    enum Freq
    {
        TZ_SECONDLY = 0,
        TZ_MINUTELY,
        TZ_HOURLY,
        TZ_WEEKLY,
        TZ_MONTHLY,
        TZ_YEARLY
    };
    Q_ENUM(Freq)

    explicit QiCalTzInfo(QObject *parent = nullptr);

    int offsetFrom() const;
    void setOffsetFrom(int offsetFrom);

    int offsetTo() const;
    void setOffsetTo(int offsetTo);

    QString tzName() const;
    void setTzName(const QString &tzName);

    QDateTime dtStart() const;
    void setDtStart(const QDateTime &dtStart);

    Freq freq() const;
    void setFreq(const Freq &freq);

    int monthStart() const;
    void setMonthStart(int monthStart);

    QString dayStart() const;
    void setDayStart(const QString &dayStart);

signals:
    void offsetFromChanged();
    void offsetToChanged();
    void tzNameChanged();
    void dtStartChanged();
    void freqChanged();
    void monthStartChanged();
    void dayStartChanged();

private:
    int m_offsetFrom;
    int m_offsetTo;
    QString m_tzName;
    QDateTime m_dtStart;
    Freq m_freq;
    int m_monthStart;
    QString m_dayStart;

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

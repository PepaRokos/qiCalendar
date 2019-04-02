#ifndef QICALRULE_H
#define QICALRULE_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>

class QiCalEvent;

class QiCalRule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Freq freq READ freq WRITE setFreq NOTIFY freqChanged)
    Q_PROPERTY(QDateTime until READ until WRITE setUntil NOTIFY untilChanged)
    Q_PROPERTY(qint32 count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(qint32 interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(QList<qint32> bySecond READ bySecond WRITE setBySecond NOTIFY bySecondChanged)
    Q_PROPERTY(QString secondList READ secondList WRITE setSecondList)
    Q_PROPERTY(QList<qint32> byMinute READ byMinute WRITE setByMinute NOTIFY byMinuteChanged)
    Q_PROPERTY(QString minuteList READ minuteList WRITE setMinuteList)
    Q_PROPERTY(QList<qint32> byHour READ byHour WRITE setByHour NOTIFY byHourChanged)
    Q_PROPERTY(QString hourList READ hourList WRITE setHourList)
    Q_PROPERTY(QList<QString> byDay READ byDay WRITE setByDay NOTIFY byDayChanged)
    Q_PROPERTY(QString dayList READ dayList WRITE setDayList)
    Q_PROPERTY(QList<qint32> byMonthDay READ byMonthDay WRITE setByMonthDay NOTIFY byMonthDayChanged)
    Q_PROPERTY(QString monthDayList READ monthDayList WRITE setMonthDayList)
    Q_PROPERTY(QList<qint32> byYearDay READ byYearDay WRITE setByYearDay NOTIFY byYearDayChanged)
    Q_PROPERTY(QString yearDayList READ yearDayList WRITE setYearDayList)
    Q_PROPERTY(QList<qint32> byWeekNo READ byWeekNo WRITE setByWeekNo NOTIFY byWeekNoChanged)
    Q_PROPERTY(QString weekList READ weekList WRITE setWeekList)
    Q_PROPERTY(QList<qint32> byMonth READ byMonth WRITE setByMonth NOTIFY byMonthChanged)
    Q_PROPERTY(QString monthList READ monthList WRITE setMonthList)
    Q_PROPERTY(QList<qint32> bySetPos READ bySetPos WRITE setBySetPos NOTIFY bySetPosChanged)
    Q_PROPERTY(QString setposList READ setposList WRITE setSetposList)
    Q_PROPERTY(QString wkst READ wkst WRITE setWkst NOTIFY wkstChanged)
    Q_PROPERTY(QiCalEvent* calEvent READ calEvent WRITE setCalEvent NOTIFY calEventChanged)
public:
    enum Freq
    {
        RR_SECONDLY = 0,
        RR_MINUTELY,
        RR_HOURLY,
        RR_DAILY,
        RR_WEEKLY,
        RR_MONTHLY,
        RR_YEARLY
    };
    Q_ENUM(Freq)

    explicit QiCalRule(QObject *parent = nullptr);

    Freq freq() const;
    void setFreq(const Freq &freq);

    QDateTime until() const;
    void setUntil(const QDateTime &until);

    qint32 count() const;
    void setCount(const qint32 &count);

    qint32 interval() const;
    void setInterval(const qint32 &interval);

    QList<qint32> bySecond() const;
    void setBySecond(const QList<qint32> &bySecond);
    void setSecondList(const QString& seconds);
    QString secondList() const;

    QList<qint32> byMinute() const;
    void setByMinute(const QList<qint32> &byMinute);
    void setMinuteList(const QString& minute);
    QString minuteList() const;

    QList<qint32> byHour() const;
    void setByHour(const QList<qint32> &byHour);
    void setHourList(const QString& hour);
    QString hourList() const;

    QList<QString> byDay() const;
    void setByDay(const QList<QString> &byDay);
    void setDayList(const QString& day);
    QString dayList() const;

    QList<qint32> byMonthDay() const;
    void setByMonthDay(const QList<qint32> &byMonthDay);
    void setMonthDayList(const QString& month);
    QString monthDayList() const;

    QList<qint32> byYearDay() const;
    void setByYearDay(const QList<qint32> &byYearDay);
    void setYearDayList(const QString& year);
    QString yearDayList() const;

    QList<qint32> byWeekNo() const;
    void setByWeekNo(const QList<qint32> &byWeekNo);
    void setWeekList(const QString& week);
    QString weekList() const;

    QList<qint32> byMonth() const;
    void setByMonth(const QList<qint32> &byMonth);
    void setMonthList(const QString& month);
    QString monthList() const;

    QList<qint32> bySetPos() const;
    void setBySetPos(const QList<qint32> &bySetPos);
    void setSetposList(const QString& pos);
    QString setposList() const;

    QString wkst() const;
    void setWkst(const QString &wkst);

    QiCalEvent *calEvent() const;
    void setCalEvent(QiCalEvent *event);

signals:
    void freqChanged();
    void untilChanged();
    void countChanged();
    void intervalChanged();
    void bySecondChanged();
    void byMinuteChanged();
    void byHourChanged();
    void byDayChanged();
    void byMonthDayChanged();
    void byYearDayChanged();
    void byWeekNoChanged();
    void byMonthChanged();
    void bySetPosChanged();
    void wkstChanged();
    void calEventChanged();

private:
    Freq m_freq;
    QDateTime m_until;
    qint32 m_count;
    qint32 m_interval;
    QList<qint32> m_bySecond;
    QList<qint32> m_byMinute;
    QList<qint32> m_byHour;
    QList<QString> m_byDay;
    QList<qint32> m_byMonthDay;
    QList<qint32> m_byYearDay;
    QList<qint32> m_byWeekNo;
    QList<qint32> m_byMonth;
    QList<qint32> m_bySetPos;
    QString m_wkst;
    QiCalEvent *m_event;

    void fillIntList(const QString& strList, QList<qint32>& list);
    QString getIntList(const QList<qint32>& list) const;
    QString getStringList(const QList<QString>& list) const;
};

#endif // QICALRULE_H

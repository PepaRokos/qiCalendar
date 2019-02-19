#ifndef QICALEVENT_H
#define QICALEVENT_H

#include <QObject>
#include <QDateTime>
#include <QString>

class QiCalAlarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Action action READ action WRITE setAction NOTIFY actionChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString trigger READ trigger WRITE setTrigger NOTIFY triggerChanged)

public:
    enum Action
    {
        ACT_AUDIO = 0,
        ACT_DISPLAY,
        ACT_EMAIL
    };
    Q_ENUM(Action)

    explicit QiCalAlarm(QObject *parent = nullptr);

    Action action() const;
    void setAction(const Action &action);

    QString description() const;
    void setDescription(const QString &description);

    QString trigger() const;
    void setTrigger(const QString &trigger);

signals:
    void actionChanged();
    void descriptionChanged();
    void triggerChanged();

private:
    Action m_action;
    QString m_description;
    QString m_trigger;
};

class QiCalEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dtStart READ dtStart WRITE setDtStart NOTIFY dtStartChanged)
    Q_PROPERTY(QDateTime dtEnd READ dtEnd WRITE setDtEnd NOTIFY dtEndChanged)
    Q_PROPERTY(QDateTime dtStamp READ dtStamp WRITE setDtStamp NOTIFY dtStampChanged)
    Q_PROPERTY(QString uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified NOTIFY lastModifiedChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(Transp transp READ transp WRITE setTransp NOTIFY transpChanged)
    Q_PROPERTY(QList<QiCalAlarm*> alarms READ alarms NOTIFY alarmsChanged)
public:
    enum Status
    {
        STAT_TENTATIVE = 0,
        STAT_CONFIRMED,
        STAT_CANCELLED
    };
    Q_ENUM(Status)

    enum Transp
    {
        TRANS_OPAQUE = 0,
        TRANS_TRANSPARENT
    };
    Q_ENUM(Transp)

    explicit QiCalEvent(QObject *parent = nullptr);

    QDateTime dtStart() const;
    void setDtStart(const QDateTime &dtStart);

    QDateTime dtEnd() const;
    void setDtEnd(const QDateTime &dtEnd);

    QDateTime dtStamp() const;
    void setDtStamp(const QDateTime &dtStamp);

    QString uid() const;
    void setUid(const QString &uid);

    QDateTime created() const;
    void setCreated(const QDateTime &created);

    QString description() const;
    void setDescription(const QString &description);

    QDateTime lastModified() const;
    void setLastModified(const QDateTime &lastModified);

    QString location() const;
    void setLocation(const QString &location);

    Status status() const;
    void setStatus(const Status &status);

    Transp transp() const;
    void setTransp(const Transp &transp);

    QList<QiCalAlarm *> alarms() const;
    void addAlarm(QiCalAlarm* alarm);
    void removeAlarm(QiCalAlarm* alarm);
    void clearAlarms();

    QString summary() const;
    void setSummary(const QString &summary);

    bool operator <(const QiCalEvent &other) const;

signals:
    void dtStartChanged();
    void dtEndChanged();
    void dtStampChanged();
    void uidChanged();
    void createdChanged();
    void descriptionChanged();
    void summaryChanged();
    void lastModifiedChanged();
    void locationChanged();
    void statusChanged();
    void transpChanged();
    void alarmsChanged();

private:
    QDateTime m_dtStart;
    QDateTime m_dtEnd;
    QDateTime m_dtStamp;
    QString m_uid;
    QDateTime m_created;
    QString m_description;
    QString m_summary;
    QDateTime m_lastModified;
    QString m_location;
    Status m_status;
    Transp m_transp;
    QList<QiCalAlarm*> m_alarms;
};

#endif // QICALEVENT_H

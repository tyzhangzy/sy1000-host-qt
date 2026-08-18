#ifndef LANGUAGEHELPER_H
#define LANGUAGEHELPER_H

#include <QObject>
#include <QTranslator>

class QQmlApplicationEngine;

/**
 * LanguageHelper - switch UI language at runtime (EN default / zh_CN).
 * Exposed to QML via context property "lang", e.g. lang.setLanguage("zh_CN")
 */
class LanguageHelper : public QObject
{
    Q_OBJECT
public:
    explicit LanguageHelper(QQmlApplicationEngine *engine, QObject *parent = nullptr);

    Q_INVOKABLE void setLanguage(const QString &lang);
    Q_INVOKABLE QString current() const;

Q_SIGNALS:
    void currentChanged();

private:
    QQmlApplicationEngine *m_engine;
    QTranslator m_translator;
    QString m_current = QStringLiteral("en");
};

#endif // LANGUAGEHELPER_H

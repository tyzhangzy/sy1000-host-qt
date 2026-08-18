#ifndef LANGUAGEHELPER_H
#define LANGUAGEHELPER_H

#include <QObject>
#include <QTranslator>

class QQmlApplicationEngine;

/**
 * LanguageHelper - 运行时切换界面语言（中/英）。
 * 通过 context property "lang" 暴露给 QML，例如：lang.setLanguage("zh_CN")
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

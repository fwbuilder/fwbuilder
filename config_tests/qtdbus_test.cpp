
#include <QApplication>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>

main()
{
    QApplication app();
    QDBusConnection conn = QDBusConnection::systemBus();
}

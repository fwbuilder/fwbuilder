#ifndef AUTOCOMPLETEDCOMBOBOX_H
#define AUTOCOMPLETEDCOMBOBOX_H

#include <QTimer>
#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>

class AutocompletedComboBox : public QComboBox
{
    Q_OBJECT
private:
    QCompleter *completer;
    QStringList words;
    QStringListModel model;
    QTimer timer;
public:
    AutocompletedComboBox(QWidget* parent = 0);

public slots:
    void filterUpdate(QString);
    void addWord();
};

#endif // AUTOCOMPLETEDCOMBOBOX_H

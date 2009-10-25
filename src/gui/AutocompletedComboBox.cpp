#include "AutocompletedComboBox.h"

AutocompletedComboBox::AutocompletedComboBox(QWidget* parent)
{
    this->setParent(parent);
    model.setStringList(words);
    completer = new QCompleter(&model, this);
    this->setCompleter(this->completer);
    connect(this, SIGNAL(editTextChanged(QString)),
            this, SLOT(filterUpdate(QString)));
    this->setModel(&model);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout ()), this, SLOT(addWord()));
}

void AutocompletedComboBox::filterUpdate(QString text)
{
    timer.stop();
    timer.start(1000);
}

void AutocompletedComboBox::addWord()
{
    if (this->currentText().length() < 4) return;
    if (words.contains(this->currentText())) return;
    QString text = this->currentText();
    words << text;
    this->model.setStringList(this->words);
    this->setEditText(text);
}

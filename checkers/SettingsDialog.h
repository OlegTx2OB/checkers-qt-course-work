#pragma once
#include <QDialog>
#include <QSettings>

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog();
    ~SettingsDialog();
	void loadSettings();
private slots:
	void saveSettings();
private:
    Ui::SettingsDialog *ui;
};

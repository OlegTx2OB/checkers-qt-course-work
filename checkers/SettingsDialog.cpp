#include "SettingsDialog.h"
#include "ui_settingsdialog.h"
#include "CheckersGame.h"

SettingsDialog::SettingsDialog() : ui(new Ui::SettingsDialog)
{
//setting possible options in settings
    ui->setupUi(this);
	ui->spinBox->setMinimum(3);
	ui->spinBox->setMaximum(7);
	ui->comboBox->addItem(tr("Russian"));
	ui->comboBox->addItem(tr("International"));
	ui->comboBoxColor->addItem(tr("White"));
	ui->comboBoxColor->addItem(tr("Black"));

	loadSettings();

	connect( this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
//getting game values from preferences
void SettingsDialog::loadSettings()
{
    QSettings settings("QCheckers");

    int color = settings.value("color", WHITE).toInt();

	if(color != WHITE && color !=BLACK)
		color = WHITE;

    int type = settings.value("type", RUSSIAN).toInt();

	if( type != RUSSIAN && type!= INTERNATIONAL )
		type = RUSSIAN;

	int depth = settings.value("depth",3).toInt();

    if( depth < 3 || depth > 7) depth = 3;

	if( type == INTERNATIONAL )
		ui->comboBox->setCurrentIndex(1);
	else
		ui->comboBox->setCurrentIndex(0);

    if( color == BLACK ) ui->comboBoxColor->setCurrentIndex(1);
    else ui->comboBoxColor->setCurrentIndex(0);

	ui->spinBox->setValue(depth);
}


void SettingsDialog::saveSettings()
{
    QSettings settings("QCheckers");
	int color, type, depth;

	if( ui->comboBox->currentIndex() == 0 )
		type = RUSSIAN;
	else
		type = INTERNATIONAL;
	if( ui->comboBoxColor->currentIndex() == 0)
		color = WHITE;
	else
		color = BLACK;
	depth = ui->spinBox->value();

	settings.setValue("color",color);
	settings.setValue("depth",depth);
	settings.setValue("type",type);
}

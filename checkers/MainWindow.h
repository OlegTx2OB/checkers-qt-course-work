#pragma once
#include <QMainWindow>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "SettingsDialog.h"
#include "CheckersGame.h"
#include "CheckersState.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
private:
    CheckersGame * game;
    SettingsDialog * dialog;
public:
    MainWindow();
    ~MainWindow();
private slots:

	void startNewGame();
	void endGame();
	void settings();
	void about();
};

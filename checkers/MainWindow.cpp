#include "MainWindow.h"

MainWindow::MainWindow() : QMainWindow()
{
    setupUi(this);

    game = new CheckersGame();
    dialog = new SettingsDialog();

	// Signal-Slots
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionStartNewGame, SIGNAL(triggered()), this, SLOT(startNewGame()));
	connect(actionEndGame, SIGNAL(triggered()), this, SLOT(endGame()));
    connect(actionSettings, SIGNAL(triggered()), this, SLOT(settings()));
	connect(actionAboutCheckers, SIGNAL(triggered()), this, SLOT(about()));

	connect(picture, SIGNAL(mouseClicked(int, int)), game, SLOT(setClicked(int, int)));
	connect(game, SIGNAL(stateChanged(CheckersState *)), picture, SLOT(setState(CheckersState *)));
    connect(game, SIGNAL(vectorChanged(std::vector <Point> &)), picture, SLOT(setVector(std::vector <Point> &)));
	connect(game, SIGNAL(vectorDeleted()), picture, SLOT(deleteVector()));

    resize(1920,1080);

	actionOpen->setEnabled(false);
	actionSave->setEnabled(false);
	actionEndGame->setEnabled(false);
}

MainWindow::~MainWindow()
{
	delete game;
	delete dialog;
}

void MainWindow::startNewGame()
{
    QSettings settings("QCheckers");

	actionStartNewGame->setEnabled(false);

	int type = settings.value("type",RUSSIAN).toInt();
	int color = settings.value("color",WHITE).toInt();

	int level = settings.value("depth",3).toInt();

    if(color == WHITE) color = BLACK;
    else color = WHITE;

	game->setGameType(type);
	picture->setComputerColor(color);
    game->setSearchDepth(level);
	game->startNewGame(color);

	actionEndGame->setEnabled(true);
}

void MainWindow::endGame()
{
	actionEndGame->setEnabled(false);
	game->endGame();
	picture->clear();
	actionStartNewGame->setEnabled(true);
}

void MainWindow::settings()
{
	dialog->exec();
}

void MainWindow::about()
{
    QMessageBox::about(this, trUtf8("course work OlegTx2OB"),
			   trUtf8(
    "<P align=center>qt checkers course work"
    "<P align=center>created by Oleg Poteychyk 250502"
    "<P align=center>-2023-"));
}



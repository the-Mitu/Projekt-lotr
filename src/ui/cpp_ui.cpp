/**
 * Moduł C++ stworzony do integracji logiki aplikacji napisanej w D
 * z interfejsem graficznym stworzonym w C++ i QML z użyciem bibliotek
 * Qt 5.x.
 */

#include "cpp_ui.h"

/**
 * Tworzy interfejs użytkownika.
 */
UI::UI(UICallback *callback)
{
	this->callback = callback;
	
		// Ubuntu SDK wymaga ustawienia zmiennej środowiskowej 	APP_ID.
	setenv("APP_ID", "com.komnata_mitana.lotr", 1);
	
	this->engine()->addImportPath("qml");
	setSource(QUrl("qml/ui.qml"));
	setResizeMode(QQuickView::SizeRootObjectToView);
	
	connect(rootObject(), SIGNAL(start()), this, SLOT(simulationStart()));
	connect(rootObject(), SIGNAL(stop()), this, SLOT(simulationStop()));
	connect(rootObject(), SIGNAL(step()), this, SLOT(simulationStep()));
	connect(rootObject(), SIGNAL(reset()), this, SLOT(simulationReset()));
	connect(rootObject(), SIGNAL(saveLog()), this, SLOT(saveLog()));
	connect(rootObject(), SIGNAL(zoomIn()), this, SLOT(zoomIn()));
	connect(rootObject(), SIGNAL(zoomOut()), this, SLOT(zoomOut()));
	connect(rootObject(), SIGNAL(loadSettings()), this, SLOT(loadSettings()));
	connect(rootObject(), SIGNAL(saveSettings(QString)), this, SLOT(saveSettings(QString)));
	
	show();
}

/**
 * Rozpoczyna symulację.
 */
void UI::simulationStart()
{
	callback->simulationStart();
}

/**
 * Rozpoczyna symulację.
 */
void UI::simulationStop()
{
	callback->simulationStop();
}

/**
 * Rozpoczyna symulację.
 */
void UI::simulationStep()
{
	callback->simulationStep();
}

/**
 * Resetuje symulację.
 */
void UI::simulationReset()
{
	callback->simulationReset();
}

/**
 * Zapisujemy loga.
 */
void UI::saveLog()
{
	callback->saveLog();
}

/**
 * Powiększanie
 */
void UI::zoomIn()
{
	callback->zoomIn();
}

/**
 * Pomniejszenie
 */
void UI::zoomOut()
{
	callback->zoomOut();
}

/**
 * Wczytuje ustawienia
 */
void UI::loadSettings()
{
	callback->loadSettings();
}

/**
 * Zapisuje ustawienia
 */
void UI::saveSettings(QString params)
{
	callback->saveSettings(params.toUtf8().data());
}

/**
 * Uruchamia interfejs użytkownika.
 */
void UILink::run()
{
	app->exec();
}

/**
 * Inicjalizuje planszę o zadanych wymiarach. Jeżeli wcześniej była
 * wyświetlona jakaś plansza, całkowicie ją czyści.
 * Params:
 * width = Długość planszy
 * height = Wysokość planszy
 */
void UILink::initBoard(int width, int height)
{
	// Ustawienie szerokości i wysokości
	board->setProperty("rows", height);
	board->setProperty("columns", width);
	
	// Wyczyszczenie planszy
	QMetaObject::invokeMethod(board, "clear");
}

/**
 * Dodaje pole do planszy
 */
void UILink::appendField(int y, int x, char *path)
{
	QMetaObject::invokeMethod(board, "appendField", Q_ARG(QVariant, y), Q_ARG(QVariant, x), Q_ARG(QVariant, path));
}

/**
 * Usuwa jednostkę z pola
 */
void UILink::clearField(int y, int x)
{
	stringstream stream;
	stream << "field_" << y << "_" << x;
	
	QObject *field = board->findChild<QObject*>(stream.str().c_str());
	QMetaObject::invokeMethod(field, "clear");
}

/**
 * Ustawia jednostkę w polu
 */
void UILink::insertUnit(int y, int x, char *imagePath, char *color1, char *color2)
{
	stringstream stream;
	stream << "field_" << y << "_" << x;
	
	QObject *field = board->findChild<QObject*>(stream.str().c_str());
	QMetaObject::invokeMethod(field, "insertUnit", Q_ARG(QVariant, y), Q_ARG(QVariant, x), Q_ARG(QVariant, imagePath), Q_ARG(QVariant, color1), Q_ARG(QVariant, color2));
}

/**
 * Aktualizuje licznik tur
 */
void UILink::updateTurnCount(int turnCount)
{
	QMetaObject::invokeMethod(ui->rootObject(), "updateTurnCount", Q_ARG(QVariant, turnCount));
}

/**
 * Pokazuje dialog końca bitwy
 */
void UILink::showBattleOverDialog(char* winner)
{
	QMetaObject::invokeMethod(ui->rootObject(), "showBattleOverDialog", Q_ARG(QVariant, winner));
}

/**
 * Aktualizuje loga w WebView.
 */
void UILink::log(char* html)
{
	QMetaObject::invokeMethod(ui->rootObject(), "log", Q_ARG(QVariant, html));
}

/**
 * Powiększenie
 */
void UILink::zoomIn(int y, int x)
{
	stringstream stream;
	stream << "field_" << y << "_" << x;
	
	QObject *field = board->findChild<QObject*>(stream.str().c_str());
	QMetaObject::invokeMethod(field, "zoomIn");
}

/**
 * Pomniejszenie
 */
void UILink::zoomOut(int y, int x)
{
	stringstream stream;
	stream << "field_" << y << "_" << x;
	
	QObject *field = board->findChild<QObject*>(stream.str().c_str());
	QMetaObject::invokeMethod(field, "zoomOut");
}

/**
 * Otwiera ustawienia
 */
void UILink::openSettings(char *params)
{
	QMetaObject::invokeMethod(ui->rootObject(), "openSettings", Q_ARG(QVariant, params));
}

/**
 * Zamyka ustawienia
 */
void UILink::closeSettings()
{
	QMetaObject::invokeMethod(ui->rootObject(), "closeSettings");
}

/**
 * Tworzy obiekt interfejsu oraz łącznik z językiem D.
 * 
 * Returns:
 * Obiekt sterujący interfejsem
 */
UILink *createUI(UICallback *callback)
{
	// Tworzenie łącznika
	UILink *link = new UILink();
	
	// Tworzenie aplikacji
	int argc = 0;
	char **argv = NULL;
	link->app = new QGuiApplication(argc, argv);
	
	// Tworzenie interfejsu
	UI *ui = new UI(callback);
	ui->link = link;
	link->ui = ui;
	link->board = ui->rootObject()->findChild<QObject*>("board");
	
	// Zwrócenie wskaźnika na łącznik
	return link;
}

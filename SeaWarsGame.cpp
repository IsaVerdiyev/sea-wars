#include <iostream>
#include <cstring>
#include <iomanip>
#include <Windows.h>
#include <conio.h>
#include <ctime>
#pragma warning(disable : 4996)
using namespace std;

//неизменное количество линий и столбцов 
//изменяемой части игрового поля
const int numberOfLinesAndSymols = 21;

// инициализация цветов 
enum {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};



// комбинация цветов переднего и заднего плана используемые в игре
WORD WELCOME_Sentence_Color = (WORD)White << 4 | Green;
WORD ChosenStringOfInterface_Color = (WORD)Yellow | Black;
WORD CursorOnSea_Color = (WORD)Blue << 4 | Black;
WORD ShipOnSea_Color = (WORD)Black << 4 | Brown;
WORD CursorOnShip_Color = (WORD)Black << 4 | LightCyan;
WORD ChosenShipCannotBeSettled_Color = (WORD)Red << 4 | Red;
WORD ChosenShipCanBeSettled_Color = (WORD)Green << 4 | Green;
WORD CursorOnCellOfShip_Color = (WORD)Black << 4 | Cyan;
WORD AlreadyAttackedPosition_Color = (WORD)Yellow << 4 | Black;
WORD CursorOnWoundedShip_Color = (WORD)Red << 4 | Black;
WORD WoundedShip_Color = (WORD)Cyan << 4 | Red;
WORD PlayerWin_Color = (WORD)White << 4 | Green;
WORD CompWin_Color = (WORD)White << 4 | Red;

// позиции различных чатей программы в консоли
COORD SetModeDisplayPosition = { 30, 5 };
COORD SettingShipManuallyStringsPosition = { 30, 10 };
COORD AttackButtonsMeaningPosition = { 60, 13 };
COORD fileNameEnterPosition = { 15, 27 };
COORD WinInformationPosition = { 60, 5 };
COORD EndOfGameInfoPosition = { 5, 27 };

// структура для обозначения позиции курсора относительно начала игрового поля
struct Cursor {
	int X;
	int Y;
};

// структура хранящая информацию о консоли
struct ConsoleInformation {
	HANDLE ConsoleDescriptor;
	WORD OriginalColors;
} ThisConsole;

// структура корабля
struct Ship {
	int X_coordinateOnField, Y_coordinateOnField;
	int size;
	bool isHorizontal = true;
	bool isKilled = false;
	int KilledCells = 0;
};

// структура типа  линии таблицы игрового поля
struct fieldLine {
	bool hasCells = false;
	char lineElements[numberOfLinesAndSymols];
	char firstElement, lastElement, middleElement, connectionElement;
};

// структура игрового поля
struct PlayingField {
	char ArrayOfCellsOfPlayingField[10][10];
	char columnLetters[23];
	int stringNumbers[10];
	fieldLine lines[numberOfLinesAndSymols];
	Ship OneSize[4];
	Ship TwoSize[3];
	Ship ThreeSize[2];
	Ship FourSize;
	Ship *shipArray[10];
	bool cursorIsSet = true;
	bool shipIsChosen = false;
	int chosenShipIndex;
	int *pointerToDinamicArrayOfPossiblePositions = nullptr;
	int sizeOfDinamicArrayOfPossiblePositions = 0;
	bool MapIsReady = false;
	COORD startPositionOfField;
	Cursor FieldCursor = { 0, 0 };
	bool ShipsAreHidden = false;
	Cursor positionOfWoundedShip;
	int  *pointerToDinamicArrayOfPossiblePositionsOfWoundedShip = nullptr;
	int sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip = 0;
	int sizeOfKilledShips = 0;
	int missedAttacks = 0;
	int woundedAttacks = 0;
	int *pointerToDinamicArrayOfAttackedPositionsInOneTurn = nullptr;
	int sizeOfDinamicArrayOfAttackedPositions = 0;
};

// cимволы для построения границ игрового поля
const char topRightBorderCharacter = 191;
const char bottomRightBorderCharacter = 217;
const char topLeftBorderCharacter = 218;
const char bottomLeftBorderCharacter = 192;
const char topMiddleBorderCharacter = 194;
const char bottomMiddleBorderCharacter = 193;
const char leftMiddleBorderCharacter = 195;
const char rightMiddleBorderCharacter = 180;
const char centerMiddleBorderCharacter = 197;
const char verticalLineBorderCharacter = 179;
const char horizontalLineBorderCharacter = 196;

// символы для обозначения кораблей, неизведонного участка, мимо, ранил
const char freeSeaSpaceCharacter = ' ';
const char woundedCharacter = 'X';
const char missedCharacter = 'O';
const char shipCharacter = 219;

// значения управляющих стрелок и ввода
const int arrows = 224;
const int upArrow = 72;
const int leftArrow = 75;
const int rightArrow = 77;
const int downArrow = 80;
const int rotateUpperCase = 'R';
const int rotateLowerCase = 'r';
const int enter = 13;
const int completeUpperCase = 'C';
const int completeLowerCase = 'c';
const int saveButtonUpperCase = 'S';
const int saveButtonLowerCase = 's';
const int loadButtonUpperCase = 'D';
const int loadButtonLowerCase = 'd';

//функция приобретения информации о консоли
void getConsoleInfo(ConsoleInformation &Console);

// функции создания каркаса игрового поля
void createFrameOfPlayingField(PlayingField &Field, fieldLine *lineTypes);
void giveSizeToShipsOfField(PlayingField &Field);
void createLineTypes(fieldLine &lineType);
void initializeStringNumbers(int stringNumbers[]);
void initializeFieldLines(fieldLine lines[], fieldLine *lineTypes);
void fillDefaultSymbolsInPlayingFieldCells(char seaPosition[][10]);

// функции необходимые для заполнения игрового поля кораблями ранодомно
void createInitialMapWithShips(PlayingField &Field);
void setShipsRandomly(PlayingField &Field);
void setHorizontalShipRandomly(PlayingField &Field, Ship *chosenShip, int index);
void setVerticalShipRandomly(PlayingField &Field, Ship *chosenShip, int index);

// функции для создания динамического массива позиций игрового поля и удаления определенного места из этого массива
void createInitializedDinamicArrayOfPossibleChoices(PlayingField &Field);
void initializeDinamicArrayOfPossibleChoices(PlayingField &Field);
void removeOccupiedPositionFromDinamicArray(PlayingField &Field, int pos);

// функция инициализации массива кораблей поля
void initializeArrayOfShips(PlayingField & Field);

// функции для проверки возможности размещения корабля в поле при заполении игрового поля кораблями
bool checkSetPossibilityHorizontallyOfShip(PlayingField &Field, Ship *chosenShip, int index);
bool checkSetPossibilityVerticallyOfShip(PlayingField &Field, Ship *chosenShip, int index);

// функции используемые в функциях проверки размещения корабля на поле
bool isFirstCellOfShip(int shipCellIndex);
bool isLastCellOfShip(int shipCellIndex, int shipSize);
bool isInFirstPlayingFieldColumn(int cell_Index);
bool isInLastPlayingFieldColumn(int cell_Index);
bool isInFirstPlayingFieldString(int cell_Index);
bool isInLastPlayingFieldString(int cell_Index);
bool leftCellisShipCharacter(char *pointerToCellOfField);
bool leftCellisWoundedCharacter(char *pointerToCellOfField);
bool leftCellisMissedCharacter(char *pointerToCellOfField);
bool upperLeftCellisShipCharacter(char *pointerToCellOfField);
bool bottomLeftCellisShipCharacter(char *pointerToCellOfField);
bool upperCellisShipCharacter(char *pointerToCellOfField);
bool upperCellisWoundedCharacter(char *pointerToCellOfField);
bool upperCellisMissedCharacter(char *pointerToCellOfField);
bool cellisShipCharacter(char *pointerToCellOfField);
bool cellisWoundedCharacter(char *pointerToCellOfField);
bool cellisMissedCharacter(char *pointerToCellOfField);
bool bottomCellisShipCharacter(char *pointerToCellOfField);
bool bottomCellisWoundedCharacter(char *pointerToCellOfField);
bool bottomCellisMissedCharacter(char *pointerToCellOfField);
bool rightCellisShipCharacter(char *pointerToCellOfField);
bool rightCellisWoundedCharacter(char *pointerToCellOfField);
bool rightCellisMissedCharacter(char *pointerToCellOfField);
bool upperRightCellisShipCharacter(char *pointerToCellOfField);
bool bottomRightCellisShipCharacter(char *pointerToCellOfField);
bool checkUpperStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int cell_Index);
bool checkCurrentStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int cell_Index);
bool checkBottomStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int cell_Index);
bool checkLeftColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int cell_Index);
bool checkCurrentColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int cell_Index);
bool checkRightColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int cell_Index);

//функции отображения и использования интерфейса выбора режима размещения кораблей игрока
void setModeOfCreatingPlayerFieldFromInterface(PlayingField & Field);
void showInterfaceOfChoosingModeForSettingShipsOfPlayer(PlayingField & playerField);

//функция отображения подсказок и  по использованию клавиш во время игры
void showButtonsWhenSettingShipsManually();
void showButtonsDuringGame();

// функции используемые при отображении игрового поля
void showPlayingField(PlayingField Field);
void showColumnLetters(char *letters);
void showMainFieldWithCellsLinesCursorAndAttackedPositions(PlayingField Field);
void showCellsAndLinesOfPlayingField(PlayingField &Field);
void showLineWithCellsOfPlayingField(PlayingField & Field, int & cellCounter, int indexOfFieldLine);
void showLineWithVisibleShips(fieldLine lineType, char *pointerToCellOfField, ConsoleInformation &Console);
void showLineWithHiddenShips(fieldLine lineType, char *pointerToCellOfField, ConsoleInformation &Console);
void showConnectingBorderLineOfField(fieldLine lineType);
void showCursorOnFieldDuringAttack(PlayingField Field);
void showAttackedPositionsByComputer(PlayingField Field);
void showFieldWhileSettingShipsManually(PlayingField Field);
void showChosenShipWhileSettingManually(PlayingField Field);
void showCursorOnShipWhileSettingManually(PlayingField Field);
void showCursorOnHorizontalShipDuringSettingManually(PlayingField Field);
void showCursorOnVerticalShipDuringSettingManually(PlayingField Field);
void printWithDifferentColor(char characterOfCell, WORD cellColor);
void printWithDifferentColor(const char * str, WORD Color);
void showCursorOnCellOfFieldWithSomeColor(PlayingField Field, WORD SeaPositionColor);
void showStatistics(PlayingField myField, PlayingField compField);

// функции используемые для нахождения корабля по курсору 
void MarkShipOnWhichIsCursor(PlayingField &Field);
int defineIndexOfFirstCellOfShip(PlayingField Field, int index);
int searchIndexOfShipWhichStartsInCell(PlayingField Field, int index);

// функции используемые для размещения корабля вручную
void setShipsManually(PlayingField &Field);
void moveCursorUP(PlayingField &Field);
void moveCursorLeft(PlayingField &Field);
void moveCursorRight(PlayingField &Field);
void moveCursorDown(PlayingField &Field);
void rotateShip(PlayingField &Field);
void PlaceOrTakeShipOnPlayingField(PlayingField &Field);
void setShipShipHorizontallyOnField(PlayingField & Field);
void setShipVerticallyOnField(PlayingField & Field);
void unsetShipFromField(PlayingField & Field);
void changeCellsFromShipCharacterToFreeSeaSpaceCharacter(PlayingField &Field);
void completeSettingShipsManually(PlayingField &Field);

// функции используемые во время самой игры (атак)
void attackComputerPlayingFieldByPlayer(PlayingField &playerField, PlayingField &opponentField);
void attackPlayingFieldOfPlayerByComputer(PlayingField &compField, PlayingField &playerField);
bool allShipsAreKilled(PlayingField & Field);
void searchPossiblePositionsOfWoundedShipByComputer(PlayingField & opponentField);
void searchPossiblePositionsOfHorizontalWoundedShip(PlayingField & opponentField);
void searchPossiblePositionsOfVerticallyWoundedShip(PlayingField & opponentField);
bool checkIfWoundedDuringAttack(PlayingField &opponentField);
void savePositionThatWasAttackedByComputer(PlayingField & Field);
void setPositionAsWounded(PlayingField & opponentField);
void setPositionAsMissed(PlayingField & opponentField);
bool shipIsKilled(Ship * attackedShip);
void setToZeroSizeOfDinamicArray(int *&dinArray, int &size);

// функции используемые для обводки убитого корабля
void MarkPositionsAsAlreadyAttackedAroundKilledShip(PlayingField & opponentField);
void MarkLeftColumnAsAlreadyAttacked(PlayingField & opponentField);
void MarkUpAndDownCellsAsAlreadyAttacked(PlayingField & opponentField);
void MarkRightColumnAsAlreadyAttacked(PlayingField & opponentField);
void MarkUpperStringAsAlreadyAttacked(PlayingField & opponentField);
void MarkLeftAndRightPositionsAsAlreadyAttacked(PlayingField & opponentField);
void MarkBottomStringAsAlreadyAttacked(PlayingField & opponentField);
void MarkBottomRightCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkBottomCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkBottomLeftCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkRightCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkLeftCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkUpperRightCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkUpperCellAsAlreadyAttacked(char *pointerToCellOfField);
void MarkUpperLeftCellAsAlreadyAttacked(char *pointerToCellOfField);



//функции для сохранения и загрузки игры из файлов
void saveInFile(PlayingField playerField, PlayingField compField);
void downloadFromFile(PlayingField & playerField, PlayingField & compField);
char *getNameOfFileFromConsole(const char* message);

//шаблонные функции используемые для работы с динамическим массивом
template <typename T> void add_element_in_dinamic_array(T *&current_array, int &current_ar_size, T element);
template <typename T> void delete_element_from_dinamic_array(T *&currentArray, int  &currentArSize, int index, const char *errorMessage);
bool checkIndex(int index, int currentArSize, const char *errorMessage);
template <typename T> void allocate_memory(T *&Pointer, int size);
template <typename T> void copy_elements(T *destination, T *source, int size_to_copy);
template<typename T> void delete_din_array(T *& din_array);




int main() {
	srand(time(0));
	system("color f0");
	getConsoleInfo(ThisConsole);

	fieldLine topLine, symbolsLine, bottomLine, connectionLine;

	// инициализация заполняющих символов

	//символы верхней линии игрового поля
	topLine.firstElement = topLeftBorderCharacter;
	topLine.lastElement = topRightBorderCharacter;
	topLine.middleElement = horizontalLineBorderCharacter;
	topLine.connectionElement = topMiddleBorderCharacter;

	//символы линии которая включает ячейки игрового поля
	symbolsLine.firstElement = verticalLineBorderCharacter;
	symbolsLine.lastElement = verticalLineBorderCharacter;
	symbolsLine.middleElement = 0;
	symbolsLine.connectionElement = verticalLineBorderCharacter;
	symbolsLine.hasCells = true;

	//символы линии которая находится между ячейками игрового поля
	connectionLine.firstElement = leftMiddleBorderCharacter;
	connectionLine.lastElement = rightMiddleBorderCharacter;
	connectionLine.middleElement = horizontalLineBorderCharacter;
	connectionLine.connectionElement = centerMiddleBorderCharacter;

	//символы нижней линии игрового поля
	bottomLine.firstElement = bottomLeftBorderCharacter;
	bottomLine.lastElement = bottomRightBorderCharacter;
	bottomLine.middleElement = horizontalLineBorderCharacter;
	bottomLine.connectionElement = bottomMiddleBorderCharacter;

	// создание типов линий которые будут использоваться для создания каркаса игрового поля
	createLineTypes(topLine);
	createLineTypes(symbolsLine);
	createLineTypes(bottomLine);
	createLineTypes(connectionLine);

	// создание массива типов линий
	fieldLine lineTypes[4] = { topLine, symbolsLine, connectionLine, bottomLine };

	// объявление игровых полей игрока и компьютера
	PlayingField myField, compField;
	// инициализация массива кораблей в игровых полях
	initializeArrayOfShips(myField);
	initializeArrayOfShips(compField);
	// создание каркаса игровых полей (границ)
	createFrameOfPlayingField(myField, lineTypes);
	createFrameOfPlayingField(compField, lineTypes);
	//инициализация размеров кораблей в игровых полях
	giveSizeToShipsOfField(myField);
	giveSizeToShipsOfField(compField);
	//задание начальных позиций игровых полей в консоли, где они будут размещаться
	myField.startPositionOfField = { 5, 5 };
	compField.startPositionOfField = { 30, 5 };
	//устанавливаем флаги-условия для поля компьютера 
	compField.MapIsReady = true;		// отключаем ручную установку кораблей в поле компьютера
	compField.ShipsAreHidden = true;	// на поле компьютера корабли будут спрятаны
	compField.cursorIsSet = true;		// включаем курсор на компьютере корабля

										// устанавливаем рандомно корабли в полях
	createInitialMapWithShips(myField);
	createInitialMapWithShips(compField);

	// выбираем режим растановки кораблей игрока (вручную/рандомно)
	setModeOfCreatingPlayerFieldFromInterface(myField);
	system("cls");

	// создаем и инициализируем динамический массив 
	// индексов клеток поля игрока который будет использовать компьютер при атаке 
	createInitializedDinamicArrayOfPossibleChoices(myField);

	// если выбрали ручную расстановку кораблей, то вызывается соответсвующая функция
	if (!myField.MapIsReady) {
		setShipsManually(myField);
	}
	system("cls");
	//удаляем курсор с поля игрока
	myField.cursorIsSet = false;

	// цикл самой игры
	for (;;) {
		attackComputerPlayingFieldByPlayer(myField, compField);
		if (allShipsAreKilled(compField)) {
			break;
		}
		setToZeroSizeOfDinamicArray(myField.pointerToDinamicArrayOfAttackedPositionsInOneTurn, myField.sizeOfDinamicArrayOfAttackedPositions);
		attackPlayingFieldOfPlayerByComputer(compField, myField);
		if (allShipsAreKilled(myField)) {
			break;
		}
	}
	system("cls");

	// после завершения игры делаем корабли на поля компьютера видимыми, и
	//показываем поля и статистику
	compField.cursorIsSet = false;
	compField.ShipsAreHidden = false;
	showPlayingField(myField);
	showPlayingField(compField);
	showStatistics(myField, compField);
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, EndOfGameInfoPosition);
	system("pause");
}

void showStatistics(PlayingField myField, PlayingField compField) {
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	if (allShipsAreKilled(compField)) {
		printWithDifferentColor("PLAYER WON", PlayerWin_Color);
	}
	else {
		printWithDifferentColor("COMPUTER WON", CompWin_Color);
	}
	WinInformationPosition.Y += 2;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Player statistics";
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of shots - " << compField.woundedAttacks + compField.missedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of hittings - " << compField.woundedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of misses - " << compField.missedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of killed ships by player - " << compField.sizeOfKilledShips;

	WinInformationPosition.Y += 2;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Computer statistics";
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of shots - " << myField.woundedAttacks + myField.missedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of hittings - " << myField.woundedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of misses - " << myField.missedAttacks;
	WinInformationPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, WinInformationPosition);
	cout << "Amount of killed ships by computer - " << myField.sizeOfKilledShips;
}

void setModeOfCreatingPlayerFieldFromInterface(PlayingField &Field) {
	for (;;) {
		showInterfaceOfChoosingModeForSettingShipsOfPlayer(Field);
		int button = getch();
		if (button == upArrow) {
			if (Field.MapIsReady) {
				Field.MapIsReady = false;
			}
		}
		else if (button == downArrow) {
			if (!Field.MapIsReady) {
				Field.MapIsReady = true;
			}
		}
		else if (button == enter) {
			return;
		}
	}
}

void showInterfaceOfChoosingModeForSettingShipsOfPlayer(PlayingField &playerField) {
	COORD startOfStringPosition = SetModeDisplayPosition;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startOfStringPosition);
	printWithDifferentColor("----------WELCOME TO SEA WARS----------", WELCOME_Sentence_Color);
	startOfStringPosition.Y += 5;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startOfStringPosition);
	cout << "Choose if you want to set ships manually or automatically";
	startOfStringPosition.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startOfStringPosition);
	if (!playerField.MapIsReady) {
		printWithDifferentColor("1. Set ships manually", ChosenStringOfInterface_Color);
		startOfStringPosition.Y++;
		SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startOfStringPosition);
		cout << "2. Set ships automatically";
	}
	else {
		cout << "1. Set ships manually";
		startOfStringPosition.Y++;
		SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startOfStringPosition);
		printWithDifferentColor("2. Set ships automatically", ChosenStringOfInterface_Color);
	}
}

void showButtonsWhenSettingShipsManually() {
	COORD StartPositionOfString = SettingShipManuallyStringsPosition;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "Press ENTER to set/choose ship on playing field\n";
	StartPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "Press UP/LEFT/RIGHT/DOWN arrows to move chosen ship ";
	StartPositionOfString.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "    or cursor up/left/right/down on playing field";
	StartPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "Press R or r to rotate ship from vertical ";
	StartPositionOfString.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "    position to horizontal and vice versa";
	StartPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, StartPositionOfString);
	cout << "Press C or c to exit setting ships manually and to start game";
}

void showButtonsDuringGame() {
	COORD startPositionOfString = AttackButtonsMeaningPosition;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "Press ENTER to attack position on playing field";
	startPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "Press UP/LEFT/RIGHT/DOWN arrows to move cursor";
	startPositionOfString.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "    up/left/right/down on playing field";
	startPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "Press S or s to save game in file";
	startPositionOfString.Y += 3;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "Press D or d to load game from file\n\n\n";
}

void attackPlayingFieldOfPlayerByComputer(PlayingField &compField, PlayingField &playerField) {
	setToZeroSizeOfDinamicArray(playerField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, playerField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
	for (;;) {
		int randomPosition;
		int randomIndex;
		if (playerField.shipIsChosen) {
			searchPossiblePositionsOfWoundedShipByComputer(playerField);
			randomIndex = rand() % playerField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip;
			randomPosition = playerField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip[randomIndex];
		}
		else {
			randomIndex = rand() % playerField.sizeOfDinamicArrayOfPossiblePositions;
			randomPosition = playerField.pointerToDinamicArrayOfPossiblePositions[randomIndex];
		}

		playerField.FieldCursor.X = randomPosition % 10;
		playerField.FieldCursor.Y = randomPosition / 10;
		if (checkIfWoundedDuringAttack(playerField)) {
			break;
		}
		if (allShipsAreKilled(playerField)) {
			return;
		}
	}
}



void attackComputerPlayingFieldByPlayer(PlayingField &myField, PlayingField &opponentField) {
	for (;;) {
		showPlayingField(myField);
		showPlayingField(opponentField);
		showButtonsDuringGame();
		int c = getch();
		if (c == arrows) {
			c = getch();
			if (c == upArrow) {
				moveCursorUP(opponentField);
			}
			else if (c == leftArrow) {
				moveCursorLeft(opponentField);
			}
			else if (c == rightArrow) {
				moveCursorRight(opponentField);
			}
			else if (c == downArrow) {
				moveCursorDown(opponentField);
			}
		}
		else if (c == enter) {
			if (checkIfWoundedDuringAttack(opponentField)) {
				break;
			}
			if (allShipsAreKilled(opponentField)) {
				return;
			}
		}
		else if (c == saveButtonLowerCase | c == saveButtonUpperCase) {
			saveInFile(myField, opponentField);
			system("cls");
		}
		else if (c == loadButtonLowerCase | c == loadButtonUpperCase) {
			downloadFromFile(myField, opponentField);
			system("cls");
		}
	}
}

void searchPossiblePositionsOfWoundedShipByComputer(PlayingField &opponentField) {
	int index = opponentField.positionOfWoundedShip.Y * 10 + opponentField.positionOfWoundedShip.X;
	if (!isInFirstPlayingFieldColumn(index)) {
		if (leftCellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			setToZeroSizeOfDinamicArray(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
			searchPossiblePositionsOfHorizontalWoundedShip(opponentField);
			return;
		}
		else if (!leftCellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - 1);
		}
	}
	if (!isInFirstPlayingFieldString(index)) {
		if (upperCellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			setToZeroSizeOfDinamicArray(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
			searchPossiblePositionsOfVerticallyWoundedShip(opponentField);
			return;
		}
		else if (!upperCellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - 10);
		}
	}
	if (!isInLastPlayingFieldColumn(index)) {
		if (rightCellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			setToZeroSizeOfDinamicArray(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
			searchPossiblePositionsOfHorizontalWoundedShip(opponentField);
			return;
		}
		else if (!rightCellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + 1);
		}
	}
	if (!isInLastPlayingFieldString(index)) {
		if (bottomCellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			setToZeroSizeOfDinamicArray(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
			searchPossiblePositionsOfVerticallyWoundedShip(opponentField);
			return;
		}
		else if (!bottomCellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index)) {
			add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + 10);
		}
	}
}

bool allShipsAreKilled(PlayingField &Field) {
	return Field.sizeOfKilledShips == 10;
}


void searchPossiblePositionsOfHorizontalWoundedShip(PlayingField &opponentField) {
	int index = opponentField.positionOfWoundedShip.Y * 10 + opponentField.positionOfWoundedShip.X;
	for (int i = 0; ; i++) {
		if (isInFirstPlayingFieldColumn(index - i)) {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - i);
				break;
			}
			break;
		}
		else {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - i);
				break;
			}
		}
	}
	for (int i = 0; ; i++) {
		if (isInLastPlayingFieldColumn(index + i)) {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + i);
				break;
			}
			break;
		}
		else {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + i);
				break;
			}
		}
	}
}



void searchPossiblePositionsOfVerticallyWoundedShip(PlayingField &opponentField) {
	int index = opponentField.positionOfWoundedShip.Y * 10 + opponentField.positionOfWoundedShip.X;
	for (int i = 0; ; i++) {
		if (isInFirstPlayingFieldString(index - i * 10)) {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i * 10)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i * 10)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - i * 10);
				break;
			}
			break;
		}
		else {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i * 10)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index - i * 10)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index - i * 10);
				break;
			}
		}
	}
	for (int i = 0; ; i++) {
		if (isInLastPlayingFieldString(index + i * 10)) {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + i * 10);
				break;
			}
			break;
		}
		else {
			if (cellisMissedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				break;
			}
			else if (!cellisWoundedCharacter(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				add_element_in_dinamic_array(opponentField.pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, opponentField.sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, index + i * 10);
				break;
			}
		}
	}
}

void setToZeroSizeOfDinamicArray(int *&dinArray, int &size) {
	delete_din_array(dinArray);
	size = 0;
}


bool checkIfWoundedDuringAttack(PlayingField &opponentField) {
	if (!opponentField.ShipsAreHidden) {
		savePositionThatWasAttackedByComputer(opponentField);
	}
	if (opponentField.ArrayOfCellsOfPlayingField[opponentField.FieldCursor.Y][opponentField.FieldCursor.X] == freeSeaSpaceCharacter) {
		setPositionAsMissed(opponentField);
		return true;
	}
	else if (opponentField.ArrayOfCellsOfPlayingField[opponentField.FieldCursor.Y][opponentField.FieldCursor.X] == shipCharacter) {
		setPositionAsWounded(opponentField);
	}
	return false;
}

void savePositionThatWasAttackedByComputer(PlayingField &Field) {
	add_element_in_dinamic_array(Field.pointerToDinamicArrayOfAttackedPositionsInOneTurn, Field.sizeOfDinamicArrayOfAttackedPositions, Field.FieldCursor.Y * 10 + Field.FieldCursor.X);
}

void setPositionAsWounded(PlayingField &opponentField) {
	opponentField.positionOfWoundedShip = opponentField.FieldCursor;
	MarkShipOnWhichIsCursor(opponentField);
	opponentField.shipIsChosen = true;
	opponentField.ArrayOfCellsOfPlayingField[opponentField.FieldCursor.Y][opponentField.FieldCursor.X] = woundedCharacter;
	opponentField.shipArray[opponentField.chosenShipIndex]->KilledCells++;
	if (shipIsKilled(opponentField.shipArray[opponentField.chosenShipIndex])) {
		opponentField.shipArray[opponentField.chosenShipIndex]->isKilled = true;
	}
	if (opponentField.shipArray[opponentField.chosenShipIndex]->isKilled) {
		MarkPositionsAsAlreadyAttackedAroundKilledShip(opponentField);
		opponentField.shipIsChosen = false;
		opponentField.sizeOfKilledShips++;
	}
	removeOccupiedPositionFromDinamicArray(opponentField, opponentField.FieldCursor.Y * 10 + opponentField.FieldCursor.X);
	opponentField.woundedAttacks++;
}

void setPositionAsMissed(PlayingField &opponentField) {
	opponentField.ArrayOfCellsOfPlayingField[opponentField.FieldCursor.Y][opponentField.FieldCursor.X] = missedCharacter;
	removeOccupiedPositionFromDinamicArray(opponentField, opponentField.FieldCursor.Y * 10 + opponentField.FieldCursor.X);
	opponentField.missedAttacks++;
}


bool shipIsKilled(Ship *attackedShip) {
	if (attackedShip->KilledCells == attackedShip->size) {
		return true;
	}
	return false;
}

void MarkPositionsAsAlreadyAttackedAroundKilledShip(PlayingField &opponentField) {
	if (opponentField.shipArray[opponentField.chosenShipIndex]->isHorizontal) {
		MarkUpperStringAsAlreadyAttacked(opponentField);
		MarkLeftAndRightPositionsAsAlreadyAttacked(opponentField);
		MarkBottomStringAsAlreadyAttacked(opponentField);
	}
	else {
		MarkLeftColumnAsAlreadyAttacked(opponentField);
		MarkUpAndDownCellsAsAlreadyAttacked(opponentField);
		MarkRightColumnAsAlreadyAttacked(opponentField);
	}
}

void MarkLeftColumnAsAlreadyAttacked(PlayingField &opponentField) {
	int index = opponentField.shipArray[opponentField.chosenShipIndex]->Y_coordinateOnField * 10 + opponentField.shipArray[opponentField.chosenShipIndex]->X_coordinateOnField;
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	if (!isInFirstPlayingFieldColumn(index)) {
		if (!isInFirstPlayingFieldString(index)) {
			MarkUpperLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
			removeOccupiedPositionFromDinamicArray(opponentField, index - 11);
		}
		for (int i = 0; i < chosenShip->size; i++) {
			MarkLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10);
			removeOccupiedPositionFromDinamicArray(opponentField, index + i * 10 - 1);
		}
		if (!isInLastPlayingFieldString(index + (chosenShip->size - 1) * 10)) {
			MarkBottomLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + (chosenShip->size - 1) * 10);
			removeOccupiedPositionFromDinamicArray(opponentField, index + (chosenShip->size - 1) * 10 + 9);
		}
	}
}

void MarkUpAndDownCellsAsAlreadyAttacked(PlayingField &opponentField) {
	int index = opponentField.shipArray[opponentField.chosenShipIndex]->Y_coordinateOnField * 10 + opponentField.shipArray[opponentField.chosenShipIndex]->X_coordinateOnField;
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	if (!isInFirstPlayingFieldString(index)) {
		MarkUpperCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
		removeOccupiedPositionFromDinamicArray(opponentField, index - 10);
	}
	if (!isInLastPlayingFieldString(index + (chosenShip->size - 1) * 10)) {
		MarkBottomCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + (chosenShip->size - 1) * 10 + index);
		removeOccupiedPositionFromDinamicArray(opponentField, index + (chosenShip->size - 1) * 10 + 10);
	}
}

void MarkRightColumnAsAlreadyAttacked(PlayingField &opponentField) {
	int index = opponentField.shipArray[opponentField.chosenShipIndex]->Y_coordinateOnField * 10 + opponentField.shipArray[opponentField.chosenShipIndex]->X_coordinateOnField;
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	if (!isInLastPlayingFieldColumn(index)) {
		if (!isInFirstPlayingFieldString(index)) {
			MarkUpperRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
			removeOccupiedPositionFromDinamicArray(opponentField, index - 9);
		}
		for (int i = 0; i < chosenShip->size; i++) {
			MarkRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + i * 10);
			removeOccupiedPositionFromDinamicArray(opponentField, index + i * 10 + 1);
		}
		if (!isInLastPlayingFieldString(index + (chosenShip->size - 1) * 10)) {
			MarkBottomRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + (chosenShip->size - 1) * 10);
			removeOccupiedPositionFromDinamicArray(opponentField, index + (chosenShip->size - 1) * 10 + 11);
		}
	}
}

void MarkUpperStringAsAlreadyAttacked(PlayingField &opponentField) {
	int index = opponentField.shipArray[opponentField.chosenShipIndex]->Y_coordinateOnField * 10 + opponentField.shipArray[opponentField.chosenShipIndex]->X_coordinateOnField;
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	if (!isInFirstPlayingFieldString(index)) {
		if (!isInFirstPlayingFieldColumn(index)) {
			MarkUpperLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
			removeOccupiedPositionFromDinamicArray(opponentField, index - 11);
		}
		for (int i = 0; i < chosenShip->size; i++) {
			MarkUpperCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + i);
			removeOccupiedPositionFromDinamicArray(opponentField, index + i - 10);
		}
		if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
			MarkUpperRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1);
			removeOccupiedPositionFromDinamicArray(opponentField, index + chosenShip->size - 1 - 9);
		}
	}
}

void MarkLeftAndRightPositionsAsAlreadyAttacked(PlayingField &opponentField) {
	int index = opponentField.shipArray[opponentField.chosenShipIndex]->Y_coordinateOnField * 10 + opponentField.shipArray[opponentField.chosenShipIndex]->X_coordinateOnField;
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	if (!isInFirstPlayingFieldColumn(index)) {
		MarkLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
		removeOccupiedPositionFromDinamicArray(opponentField, index - 1);
	}
	if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
		MarkRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1);
		removeOccupiedPositionFromDinamicArray(opponentField, index + chosenShip->size - 1 + 1);
	}
}

void MarkBottomStringAsAlreadyAttacked(PlayingField &opponentField) {
	Ship *chosenShip = opponentField.shipArray[opponentField.chosenShipIndex];
	int index = chosenShip->Y_coordinateOnField * 10 + chosenShip->X_coordinateOnField;
	if (!isInLastPlayingFieldString(index)) {
		if (!isInFirstPlayingFieldColumn(index)) {
			MarkBottomLeftCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index);
			removeOccupiedPositionFromDinamicArray(opponentField, index + 9);
		}
		for (int i = 0; i < chosenShip->size; i++) {
			MarkBottomCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + i);
			removeOccupiedPositionFromDinamicArray(opponentField, index + 10 + i);
		}
		if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
			MarkBottomRightCellAsAlreadyAttacked(opponentField.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1);
			removeOccupiedPositionFromDinamicArray(opponentField, index + chosenShip->size - 1 + 11);
		}
	}
}

void MarkBottomRightCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[11] = missedCharacter;
}

void MarkBottomCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[10] = missedCharacter;
}

void MarkBottomLeftCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[9] = missedCharacter;
}

void MarkRightCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[1] = missedCharacter;
}


void MarkLeftCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[-1] = missedCharacter;
}

void MarkUpperRightCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[-9] = missedCharacter;
}

void MarkUpperCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[-10] = missedCharacter;
}

void MarkUpperLeftCellAsAlreadyAttacked(char *seaPosition) {
	seaPosition[-11] = missedCharacter;

}

void setShipsManually(PlayingField &Field) {
	for (;;) {
		showPlayingField(Field);
		showButtonsWhenSettingShipsManually();
		int c = getch();
		if (c == arrows) {
			c = getch();
			if (c == upArrow) {
				moveCursorUP(Field);
			}
			else if (c == leftArrow) {
				moveCursorLeft(Field);
			}
			else if (c == rightArrow) {
				moveCursorRight(Field);
			}
			else if (c == downArrow) {
				moveCursorDown(Field);
			}
		}
		else if (c == enter) {
			PlaceOrTakeShipOnPlayingField(Field);
		}
		else if (c == rotateUpperCase || c == rotateLowerCase) {
			rotateShip(Field);
		}
		else if (c == completeLowerCase || c == completeUpperCase) {
			if (!Field.shipIsChosen) {
				completeSettingShipsManually(Field);
				return;
			}
		}
	}
}

void completeSettingShipsManually(PlayingField &Field) {
	Field.MapIsReady = true;
	Field.cursorIsSet = false;
}

void giveSizeToShipsOfField(PlayingField &Field) {
	for (int i = 0; i < 4; i++) {
		Field.OneSize[i].size = 1;
	}
	for (int i = 0; i < 3; i++) {
		Field.TwoSize[i].size = 2;
	}
	for (int i = 0; i < 2; i++) {
		Field.ThreeSize[i].size = 3;
	}
	Field.FourSize.size = 4;
}

void PlaceOrTakeShipOnPlayingField(PlayingField &Field) {
	if (cellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + Field.FieldCursor.Y * 10 + Field.FieldCursor.X)) {
		if (!Field.shipIsChosen) {
			unsetShipFromField(Field);
		}
	}
	else {
		if (Field.shipIsChosen) {
			if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
				if (checkSetPossibilityHorizontallyOfShip(Field, Field.shipArray[Field.chosenShipIndex], Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10 + Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField)) {
					setShipShipHorizontallyOnField(Field);
				}
			}
			else {
				if (checkSetPossibilityVerticallyOfShip(Field, Field.shipArray[Field.chosenShipIndex], Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10 + Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField)) {
					setShipVerticallyOnField(Field);
				}
			}
		}
	}
}

void setShipShipHorizontallyOnField(PlayingField &Field) {
	for (int i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
		Field.ArrayOfCellsOfPlayingField[Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField][Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + i] = shipCharacter;
	}
	Field.shipIsChosen = false;
}

void setShipVerticallyOnField(PlayingField &Field) {
	for (int i = 0; i < Field.shipArray[Field.chosenShipIndex][0].size; i++) {
		Field.ArrayOfCellsOfPlayingField[Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField + i][Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField] = shipCharacter;
	}
	Field.shipIsChosen = false;
}

void unsetShipFromField(PlayingField &Field) {
	MarkShipOnWhichIsCursor(Field);
	Field.shipIsChosen = true;
	changeCellsFromShipCharacterToFreeSeaSpaceCharacter(Field);
}



void rotateShip(PlayingField &Field) {
	if (Field.shipIsChosen) {
		if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
			for (int i = 0; i < Field.shipArray[Field.chosenShipIndex]->size - 1; i++) {
				if (!isLastCellOfShip(i, Field.shipArray[Field.chosenShipIndex]->size)) {
					if (isInLastPlayingFieldString(Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10 + i * 10 + Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField)) {
						return;
					}
				}
			}
			Field.shipArray[Field.chosenShipIndex]->isHorizontal = false;
		}
		else {
			for (int i = 0; i < Field.shipArray[Field.chosenShipIndex]->size - 1; i++) {
				if (!isLastCellOfShip(i, Field.shipArray[Field.chosenShipIndex]->size)) {
					if (isInLastPlayingFieldColumn(Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10 + Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + i)) {
						return;
					}
				}
			}
			Field.shipArray[Field.chosenShipIndex]->isHorizontal = true;
		}
		Field.FieldCursor.X = Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField;
		Field.FieldCursor.Y = Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField;
	}
}

void changeCellsFromShipCharacterToFreeSeaSpaceCharacter(PlayingField &Field) {
	if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
		for (int i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
			Field.ArrayOfCellsOfPlayingField[Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField][Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + i] = freeSeaSpaceCharacter;
		}
	}
	else {
		for (int i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
			Field.ArrayOfCellsOfPlayingField[Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField + i][Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField] = freeSeaSpaceCharacter;
		}
	}
}

void createLineTypes(fieldLine &lineType) {
	int beginning = 0;
	int end = 20;
	for (int i = 0; i < numberOfLinesAndSymols; i++) {
		if (i == beginning) {
			lineType.lineElements[i] = lineType.firstElement;
		}
		else if (i == end) {
			lineType.lineElements[i] = lineType.lastElement;
		}
		else if (i % 2) {
			lineType.lineElements[i] = lineType.middleElement;
		}
		else {
			lineType.lineElements[i] = lineType.connectionElement;
		}
	}
}





void createFrameOfPlayingField(PlayingField &Field, fieldLine *lineTypes) {
	initializeFieldLines(Field.lines, lineTypes);
	strcpy(Field.columnLetters, "   A B C D E F G H I J");
	initializeStringNumbers(Field.stringNumbers);
	fillDefaultSymbolsInPlayingFieldCells(Field.ArrayOfCellsOfPlayingField);
}

void fillDefaultSymbolsInPlayingFieldCells(char seaPosition[][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			seaPosition[i][j] = freeSeaSpaceCharacter;
		}
	}
}

void initializeStringNumbers(int stringNumbers[]) {
	for (int i = 0; i < 10; i++) {
		stringNumbers[i] = i + 1;
	}
}

void initializeFieldLines(fieldLine lines[], fieldLine *lineTypes) {
	int beginning = 0;
	int end = 20;
	for (int i = 0; i < numberOfLinesAndSymols; i++) {
		if (i == beginning) {
			lines[i] = lineTypes[0];
		}
		else if (i == end) {
			lines[i] = lineTypes[3];
		}
		else if (i % 2) {
			lines[i] = lineTypes[1];
		}
		else {
			lines[i] = lineTypes[2];
		}
	}
}


void showConnectingBorderLineOfField(fieldLine lineType) {
	cout << "  ";
	for (int i = 0; i < numberOfLinesAndSymols; i++) {
		cout << lineType.lineElements[i];
	}
}

void showLineWithVisibleShips(fieldLine lineType, char *seaPosition, ConsoleInformation &Console) {
	int positionCounter = 0;
	for (int i = 0; i < numberOfLinesAndSymols; i++) {
		if (lineType.lineElements[i]) {
			cout << lineType.lineElements[i];
		}
		else {
			if (seaPosition[positionCounter] == freeSeaSpaceCharacter) {
				cout << seaPosition[positionCounter++];
			}
			else if (seaPosition[positionCounter] == woundedCharacter) {
				printWithDifferentColor(seaPosition[positionCounter++], WoundedShip_Color);
			}
			else if (seaPosition[positionCounter] == missedCharacter) {
				printWithDifferentColor(seaPosition[positionCounter++], AlreadyAttackedPosition_Color);
			}
			else if (seaPosition[positionCounter] == shipCharacter) {
				printWithDifferentColor(seaPosition[positionCounter++], ShipOnSea_Color);
			}
		}
	}
}

void showLineWithHiddenShips(fieldLine lineType, char *seaPosition, ConsoleInformation &Console) {
	int positionCounter = 0;
	for (int i = 0; i < numberOfLinesAndSymols; i++) {
		if (lineType.lineElements[i]) {
			cout << lineType.lineElements[i];
		}
		else {
			if (seaPosition[positionCounter] == freeSeaSpaceCharacter || seaPosition[positionCounter] == shipCharacter) {
				cout << freeSeaSpaceCharacter;
				positionCounter++;
			}
			else if (seaPosition[positionCounter] == woundedCharacter) {
				printWithDifferentColor(seaPosition[positionCounter], WoundedShip_Color);
				positionCounter++;
			}
			else if (seaPosition[positionCounter] == missedCharacter) {
				printWithDifferentColor(seaPosition[positionCounter], AlreadyAttackedPosition_Color);
				positionCounter++;
			}
		}
	}
}



void showColumnLetters(char *letters) {
	for (int i = 0; i < 23; i++) {
		cout << letters[i];
	}
}

void showPlayingField(PlayingField Field) {
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, Field.startPositionOfField);
	showColumnLetters(Field.columnLetters);
	showMainFieldWithCellsLinesCursorAndAttackedPositions(Field);
}

void printWithDifferentColor(char symbol, WORD seaPositionColor) {
	SetConsoleTextAttribute(ThisConsole.ConsoleDescriptor, seaPositionColor);
	cout << symbol;
	SetConsoleTextAttribute(ThisConsole.ConsoleDescriptor, ThisConsole.OriginalColors);
}

void printWithDifferentColor(const char *str, WORD Color) {
	SetConsoleTextAttribute(ThisConsole.ConsoleDescriptor, Color);
	cout << str;
	SetConsoleTextAttribute(ThisConsole.ConsoleDescriptor, ThisConsole.OriginalColors);
}

void getConsoleInfo(ConsoleInformation &Console) {
	Console.ConsoleDescriptor = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO ScreenInfo;
	GetConsoleScreenBufferInfo(Console.ConsoleDescriptor, &ScreenInfo);
	Console.OriginalColors = ScreenInfo.wAttributes;
}

void showMainFieldWithCellsLinesCursorAndAttackedPositions(PlayingField Field) {
	showCellsAndLinesOfPlayingField(Field);
	if (!Field.MapIsReady) {
		showFieldWhileSettingShipsManually(Field);
	}
	else if (Field.cursorIsSet) {
		showCursorOnFieldDuringAttack(Field);
	}
	else {
		showAttackedPositionsByComputer(Field);
	}
}


void showAttackedPositionsByComputer(PlayingField Field) {
	for (int i = 0; i < Field.sizeOfDinamicArrayOfAttackedPositions; i++) {
		Field.FieldCursor.X = Field.pointerToDinamicArrayOfAttackedPositionsInOneTurn[i] % 10;
		Field.FieldCursor.Y = Field.pointerToDinamicArrayOfAttackedPositionsInOneTurn[i] / 10;
		showCursorOnFieldDuringAttack(Field);
	}
}

void showCursorOnFieldDuringAttack(PlayingField Field) {
	if (Field.ArrayOfCellsOfPlayingField[Field.FieldCursor.Y][Field.FieldCursor.X] == freeSeaSpaceCharacter || Field.ArrayOfCellsOfPlayingField[Field.FieldCursor.Y][Field.FieldCursor.X] == shipCharacter) {
		showCursorOnCellOfFieldWithSomeColor(Field, CursorOnSea_Color);
	}
	else {
		showCursorOnCellOfFieldWithSomeColor(Field, CursorOnWoundedShip_Color);
	}
}



void showFieldWhileSettingShipsManually(PlayingField Field) {
	if (Field.shipIsChosen) {
		showChosenShipWhileSettingManually(Field);
	}
	else if (Field.ArrayOfCellsOfPlayingField[Field.FieldCursor.Y][Field.FieldCursor.X] == freeSeaSpaceCharacter) {
		showCursorOnCellOfFieldWithSomeColor(Field, CursorOnSea_Color);
	}
	else {
		showCursorOnShipWhileSettingManually(Field);
	}
}

void showCursorOnCellOfFieldWithSomeColor(PlayingField Field, WORD SeaPositionColor) {
	COORD StartPositionOfCursor = { Field.startPositionOfField.X + 3, Field.startPositionOfField.Y + 2 };
	DWORD someparameter;
	FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, SeaPositionColor, 1, { StartPositionOfCursor.X + (short)(Field.FieldCursor.X * 2), StartPositionOfCursor.Y + (short)(Field.FieldCursor.Y * 2) }, &someparameter);
}

void showCursorOnShipWhileSettingManually(PlayingField Field) {
	DWORD someparameter;
	COORD StartPositionOfCursor = { Field.startPositionOfField.X + 3, Field.startPositionOfField.Y + 2 };
	MarkShipOnWhichIsCursor(Field);
	if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
		showCursorOnHorizontalShipDuringSettingManually(Field);
	}
	else {
		showCursorOnVerticalShipDuringSettingManually(Field);
	}
	FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, CursorOnCellOfShip_Color, 1, { StartPositionOfCursor.X + (short)(Field.FieldCursor.X * 2), StartPositionOfCursor.Y + (short)(Field.FieldCursor.Y * 2) }, &someparameter);
}

void showCursorOnHorizontalShipDuringSettingManually(PlayingField Field) {
	COORD StartPositionOfCursor = { Field.startPositionOfField.X + 3, Field.startPositionOfField.Y + 2 };
	DWORD someparameter;
	for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
		FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, CursorOnShip_Color, 1, { StartPositionOfCursor.X + i * 2 + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
	}
}

void showCursorOnVerticalShipDuringSettingManually(PlayingField Field) {
	COORD StartPositionOfCursor = { Field.startPositionOfField.X + 3, Field.startPositionOfField.Y + 2 };
	DWORD someparameter;
	for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
		FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, CursorOnShip_Color, 1, { StartPositionOfCursor.X + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (i * 2) + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
	}
}

void showChosenShipWhileSettingManually(PlayingField Field) {
	COORD StartPositionOfCursor = { Field.startPositionOfField.X + 3, Field.startPositionOfField.Y + 2 };
	DWORD someparameter;
	if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
		if (checkSetPossibilityHorizontallyOfShip(Field, Field.shipArray[Field.chosenShipIndex], Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10)) {
			for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
				FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, ChosenShipCanBeSettled_Color, 1, { StartPositionOfCursor.X + i * 2 + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
			}
		}
		else {
			for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
				FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, ChosenShipCannotBeSettled_Color, 1, { StartPositionOfCursor.X + i * 2 + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
			}
		}
	}
	else {
		if (checkSetPossibilityVerticallyOfShip(Field, Field.shipArray[Field.chosenShipIndex], Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10)) {
			for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
				FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, ChosenShipCanBeSettled_Color, 1, { StartPositionOfCursor.X + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (i * 2) + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
			}
		}
		else {
			for (short i = 0; i < Field.shipArray[Field.chosenShipIndex]->size; i++) {
				FillConsoleOutputAttribute(ThisConsole.ConsoleDescriptor, ChosenShipCannotBeSettled_Color, 1, { StartPositionOfCursor.X + (short)Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField * 2, StartPositionOfCursor.Y + (i * 2) + (short)Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 2 }, &someparameter);
			}
		}
	}
}

void showCellsAndLinesOfPlayingField(PlayingField &Field) {
	COORD firstLinePosition = { Field.startPositionOfField.X, Field.startPositionOfField.Y + 1 };
	COORD startPositionOfNumbers;
	startPositionOfNumbers.X = firstLinePosition.X;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, firstLinePosition);
	int cellCounter = 0;
	for (int fieldLineCounter = 0; fieldLineCounter < numberOfLinesAndSymols; fieldLineCounter++) {
		startPositionOfNumbers.Y = firstLinePosition.Y + fieldLineCounter;
		SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfNumbers);
		if (Field.lines[fieldLineCounter].hasCells) {
			showLineWithCellsOfPlayingField(Field, cellCounter, fieldLineCounter);
		}
		else {
			showConnectingBorderLineOfField(Field.lines[fieldLineCounter]);
		}
	}
}

void showLineWithCellsOfPlayingField(PlayingField &Field, int &cellCounter, int indexOfFieldLine) {
	cout << setw(2) << Field.stringNumbers[cellCounter];
	if (Field.ShipsAreHidden) {
		showLineWithHiddenShips(Field.lines[indexOfFieldLine], Field.ArrayOfCellsOfPlayingField[cellCounter++], ThisConsole);
	}
	else {
		showLineWithVisibleShips(Field.lines[indexOfFieldLine], Field.ArrayOfCellsOfPlayingField[cellCounter++], ThisConsole);
	}
}



int defineIndexOfFirstCellOfShip(PlayingField Field, int index) {
	for (;;) {
		if (!isInFirstPlayingFieldString(index)) {
			if (upperCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index) | upperCellisWoundedCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				index -= 10;
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}
	for (;;) {
		if (!isInFirstPlayingFieldColumn(index)) {
			if (leftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index) | leftCellisWoundedCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				index--;
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}
	return index;
}

int searchIndexOfShipWhichStartsInCell(PlayingField Field, int index) {
	for (int i = 0; i < 10; i++) {
		int shipPosition = Field.shipArray[i]->Y_coordinateOnField * 10 + Field.shipArray[i]->X_coordinateOnField;
		if (shipPosition == index) {
			return i;
		}
	}
}



void MarkShipOnWhichIsCursor(PlayingField &Field) {
	int startOfShip = defineIndexOfFirstCellOfShip(Field, Field.FieldCursor.Y * 10 + Field.FieldCursor.X);
	Field.chosenShipIndex = searchIndexOfShipWhichStartsInCell(Field, startOfShip);
}


void moveCursorUP(PlayingField &Field) {
	if (Field.shipIsChosen && !Field.MapIsReady) {
		if (!Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
			int firstShipCellIndex = Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10;
			if (isInFirstPlayingFieldString(firstShipCellIndex)) {
				return;
			}
		}
		else {
			if (!Field.FieldCursor.Y) {
				return;
			}
		}
		Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField--;
		Field.FieldCursor.Y--;
	}
	else {
		if (!Field.FieldCursor.Y) {
			return;
		}
		Field.FieldCursor.Y--;
	}
}

void moveCursorLeft(PlayingField &Field) {

	if (Field.shipIsChosen && !Field.MapIsReady) {

		if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
			int firstShipCellIndex = Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10;
			if (isInFirstPlayingFieldColumn(firstShipCellIndex)) {
				return;
			}
		}
		else {
			if (isInFirstPlayingFieldColumn(Field.FieldCursor.X)) {
				return;
			}
		}
		Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField--;
		Field.FieldCursor.X--;

	}
	else {
		if (isInFirstPlayingFieldColumn(Field.FieldCursor.X)) {
			return;
		}
		Field.FieldCursor.X--;
	}
}

void moveCursorRight(PlayingField &Field) {

	if (Field.shipIsChosen && !Field.MapIsReady) {

		if (Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
			int lastShipCellIndex = Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + Field.shipArray[Field.chosenShipIndex]->size - 1 + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10;
			if (isInLastPlayingFieldColumn(lastShipCellIndex)) {
				return;
			}
		}
		else {
			if (isInLastPlayingFieldColumn(Field.FieldCursor.X)) {
				return;
			}
		}
		Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField++;
		Field.FieldCursor.X++;

	}
	else {
		if (isInLastPlayingFieldColumn(Field.FieldCursor.X)) {
			return;
		}
		Field.FieldCursor.X++;
	}
}

void moveCursorDown(PlayingField &Field) {
	if (Field.shipIsChosen && !Field.MapIsReady) {

		if (!Field.shipArray[Field.chosenShipIndex]->isHorizontal) {
			int lastShipCellIndex = Field.shipArray[Field.chosenShipIndex]->X_coordinateOnField + (Field.shipArray[Field.chosenShipIndex]->size - 1) * 10 + Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField * 10;
			if (isInLastPlayingFieldString(lastShipCellIndex)) {
				return;
			}
		}
		else {
			if (Field.FieldCursor.Y == 9) {
				return;
			}
		}
		Field.shipArray[Field.chosenShipIndex]->Y_coordinateOnField++;
		Field.FieldCursor.Y++;

	}
	else {
		if (Field.FieldCursor.Y == 9) {
			return;
		}
		Field.FieldCursor.Y++;
	}
}


void createInitialMapWithShips(PlayingField &Field) {
	createInitializedDinamicArrayOfPossibleChoices(Field);
	setShipsRandomly(Field);
	setToZeroSizeOfDinamicArray(Field.pointerToDinamicArrayOfPossiblePositions, Field.sizeOfDinamicArrayOfPossiblePositions);
}

void createInitializedDinamicArrayOfPossibleChoices(PlayingField &Field) {
	Field.pointerToDinamicArrayOfPossiblePositions = nullptr;
	Field.sizeOfDinamicArrayOfPossiblePositions = 100;
	allocate_memory(Field.pointerToDinamicArrayOfPossiblePositions, Field.sizeOfDinamicArrayOfPossiblePositions);
	initializeDinamicArrayOfPossibleChoices(Field);
}

void initializeDinamicArrayOfPossibleChoices(PlayingField &Field) {
	for (int i = 0; i < Field.sizeOfDinamicArrayOfPossiblePositions; i++) {
		Field.pointerToDinamicArrayOfPossiblePositions[i] = i;
	}
}

void setShipsRandomly(PlayingField &Field) {
	for (int i = 0; i < 10;) {
		Field.shipArray[i]->isHorizontal = rand() % 2;
		int randomIndexOfPossiblePosisionsArray = rand() % Field.sizeOfDinamicArrayOfPossiblePositions;
		int randomPossiblePositionFromFreePositionsArray = Field.pointerToDinamicArrayOfPossiblePositions[randomIndexOfPossiblePosisionsArray];
		if (Field.shipArray[i]->isHorizontal) {
			if (checkSetPossibilityHorizontallyOfShip(Field, Field.shipArray[i], randomPossiblePositionFromFreePositionsArray)) {
				Field.shipArray[i]->X_coordinateOnField = randomPossiblePositionFromFreePositionsArray % 10;
				Field.shipArray[i]->Y_coordinateOnField = randomPossiblePositionFromFreePositionsArray / 10;
				setHorizontalShipRandomly(Field, Field.shipArray[i], randomPossiblePositionFromFreePositionsArray);
				i++;
			}
		}
		else {
			if (checkSetPossibilityVerticallyOfShip(Field, Field.shipArray[i], randomPossiblePositionFromFreePositionsArray)) {
				Field.shipArray[i]->X_coordinateOnField = randomPossiblePositionFromFreePositionsArray % 10;
				Field.shipArray[i]->Y_coordinateOnField = randomPossiblePositionFromFreePositionsArray / 10;
				setVerticalShipRandomly(Field, Field.shipArray[i], randomPossiblePositionFromFreePositionsArray);
				i++;
			}
		}
	}
}

bool checkSetPossibilityHorizontallyOfShip(PlayingField &Field, Ship *chosenShip, int index) {
	if (!checkCurrentStringForSettingShipHorizontally(Field, chosenShip, index)) {
		return false;
	}

	if (!checkUpperStringForSettingShipHorizontally(Field, chosenShip, index)) {
		return false;
	}

	if (!checkBottomStringForSettingShipHorizontally(Field, chosenShip, index)) {
		return false;
	}
	return true;
}

bool checkSetPossibilityVerticallyOfShip(PlayingField &Field, Ship *chosenShip, int index) {
	if (!checkCurrentColumnForSettingShipVertically(Field, chosenShip, index)) {
		return false;
	}

	if (!checkLeftColumnForSettingShipVertically(Field, chosenShip, index)) {
		return false;
	}

	if (!checkRightColumnForSettingShipVertically(Field, chosenShip, index)) {
		return false;
	}

	return true;
}

bool checkCurrentColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInFirstPlayingFieldString(index)) {
		if (upperCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
			return false;
		}
	}
	for (int i = 0; i < chosenShip->size; i++) {
		if (!isFirstCellOfShip(i)) {
			if (isInLastPlayingFieldString(index + (i - 1) * 10)) {
				return false;
			}
		}
		if (cellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
			return false;
		}
	}
	if (!isInLastPlayingFieldString(index + (chosenShip->size - 1) * 10)) {
		if (bottomCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + (chosenShip->size - 1) * 10)) {
			return false;
		}
	}
	return true;
}

bool checkLeftColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInFirstPlayingFieldColumn(index)) {
		if (!isInFirstPlayingFieldString(index)) {
			if (upperLeftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				return false;
			}
		}
		for (int i = 0; i < chosenShip->size; i++) {
			if (leftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				return false;
			}
		}
		if (!isInLastPlayingFieldString(index)) {
			if (bottomLeftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + (chosenShip->size - 1) * 10)) {
				return false;
			}
		}
	}
	return true;
}

bool checkRightColumnForSettingShipVertically(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInLastPlayingFieldColumn(index)) {
		if (!isInFirstPlayingFieldString(index)) {
			if (upperRightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				return false;
			}
		}
		for (int i = 0; i < chosenShip->size; i++) {
			if (rightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i * 10)) {
				return false;
			}
		}
		if (!isInLastPlayingFieldString(index + (chosenShip->size - 1) * 10)) {
			if (bottomRightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + (chosenShip->size - 1) * 10)) {
				return false;
			}
		}
	}
	return true;
}

bool checkCurrentStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInFirstPlayingFieldColumn(index)) {
		if (leftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
			return false;
		}
	}
	for (int i = 0; i < chosenShip->size; i++) {
		if (!isFirstCellOfShip(i)) {
			if (isInFirstPlayingFieldColumn(index + i)) {
				return false;
			}
		}
		if (cellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i)) {
			return false;
		}
	}
	if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
		if (rightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1)) {
			return false;
		}
	}
	return true;
}

bool checkUpperStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInFirstPlayingFieldString(index)) {
		if (!isInFirstPlayingFieldColumn(index)) {
			if (upperLeftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				return false;
			}
		}
		for (int i = 0; i < chosenShip->size; i++) {
			if (upperCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i)) {
				return false;
			}
		}
		if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
			if (upperRightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1)) {
				return false;
			}
		}
	}
	return true;
}

bool checkBottomStringForSettingShipHorizontally(PlayingField &Field, Ship *chosenShip, int index) {
	if (!isInLastPlayingFieldString(index)) {
		if (!isInFirstPlayingFieldColumn(index)) {
			if (bottomLeftCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index)) {
				return false;
			}
		}
		for (int i = 0; i < chosenShip->size; i++) {
			if (bottomCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + i)) {
				return false;
			}
		}
		if (!isInLastPlayingFieldColumn(index + chosenShip->size - 1)) {
			if (bottomRightCellisShipCharacter(Field.ArrayOfCellsOfPlayingField[0] + index + chosenShip->size - 1)) {
				return false;
			}
		}
	}
	return true;
}



bool isFirstCellOfShip(int shipCellIndex) {
	return (!shipCellIndex);
}

bool isInFirstPlayingFieldColumn(int seaPositionIndex) {
	return (!(seaPositionIndex % 10));
}

bool isInLastPlayingFieldColumn(int seaPositionIndex) {
	return (seaPositionIndex % 10 == 9);
}

bool leftCellisShipCharacter(char *seaPosition) {
	return (seaPosition[-1] == shipCharacter);
}

bool leftCellisWoundedCharacter(char *seaPosition) {
	return (seaPosition[-1] == woundedCharacter);
}

bool leftCellisMissedCharacter(char *seaPosition) {
	return (seaPosition[-1] == missedCharacter);
}

bool isInFirstPlayingFieldString(int seaPositionIndex) {
	return (seaPositionIndex < 10);
}

bool isInLastPlayingFieldString(int seaPositionIndex) {
	return (seaPositionIndex >= 90);
}



bool upperLeftCellisShipCharacter(char *seaPosition) {
	return (seaPosition[-11] == shipCharacter);
}



bool bottomLeftCellisShipCharacter(char *seaPosition) {
	return (seaPosition[9] == shipCharacter);
}

bool upperCellisShipCharacter(char *seaPosition) {
	return(seaPosition[-10] == shipCharacter);
}

bool upperCellisWoundedCharacter(char *seaPosition) {
	return(seaPosition[-10] == woundedCharacter);
}

bool upperCellisMissedCharacter(char *seaPosition) {
	return (seaPosition[-10] == missedCharacter);
}

bool cellisShipCharacter(char *seaPosition) {
	return (seaPosition[0] == shipCharacter);
}

bool cellisWoundedCharacter(char *seaPosition) {
	return (seaPosition[0] == woundedCharacter);
}

bool cellisMissedCharacter(char *seaPosition) {
	return (seaPosition[0] == missedCharacter);
}

bool bottomCellisShipCharacter(char *seaPosition) {
	return (seaPosition[10] == shipCharacter);
}

bool bottomCellisWoundedCharacter(char *seaPosition) {
	return (seaPosition[10] == woundedCharacter);
}

bool bottomCellisMissedCharacter(char *seaPosition) {
	return (seaPosition[10] == missedCharacter);
}

bool isLastCellOfShip(int shipCellIndex, int shipSize) {
	return (shipCellIndex == shipSize - 1);
}

bool rightCellisShipCharacter(char *seaPosition) {
	return (seaPosition[1] == shipCharacter);
}

bool rightCellisWoundedCharacter(char *seaPosition) {
	return (seaPosition[1] == woundedCharacter);
}

bool rightCellisMissedCharacter(char *seaPosition) {
	return  (seaPosition[1] == missedCharacter);
}

bool upperRightCellisShipCharacter(char *seaPosition) {
	return (seaPosition[-9] == shipCharacter);
}

bool bottomRightCellisShipCharacter(char *seaPosition) {
	return (seaPosition[11] == shipCharacter);
}

void setHorizontalShipRandomly(PlayingField &Field, Ship *chosenShip, int index) {
	for (int i = 0; i < chosenShip->size; i++) {
		Field.ArrayOfCellsOfPlayingField[0][index + i] = shipCharacter;
		removeOccupiedPositionFromDinamicArray(Field, index + i);
	}
}

void setVerticalShipRandomly(PlayingField &Field, Ship *chosenShip, int index) {
	for (int i = 0; i < chosenShip->size; i++) {
		Field.ArrayOfCellsOfPlayingField[0][index + (i * 10)] = shipCharacter;
		removeOccupiedPositionFromDinamicArray(Field, index + i * 10);
	}
}


void removeOccupiedPositionFromDinamicArray(PlayingField &Field, int pos) {
	for (int i = 0; i < Field.sizeOfDinamicArrayOfPossiblePositions; i++) {
		if (Field.pointerToDinamicArrayOfPossiblePositions[i] == pos) {
			delete_element_from_dinamic_array(Field.pointerToDinamicArrayOfPossiblePositions, Field.sizeOfDinamicArrayOfPossiblePositions, ++i, "Deleting error\n");
			return;
		}
	}
}

void saveInFile(PlayingField playerField, PlayingField compField) {
	COORD startPositionOfString = fileNameEnterPosition;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	char *fileName = getNameOfFileFromConsole("Enter direction to file in which you want to save game: ");
	FILE *saveFile = fopen(fileName, "wb");
	delete[] fileName;
	PlayingField arrayOfFields[2] = { playerField, compField };
	for (int i = 0; i < 2; i++) {
		fwrite(arrayOfFields + i, sizeof(PlayingField), 1, saveFile);
		fwrite(arrayOfFields[i].pointerToDinamicArrayOfPossiblePositions, sizeof(int), arrayOfFields[i].sizeOfDinamicArrayOfPossiblePositions, saveFile);
		fwrite(arrayOfFields[i].pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, sizeof(int), arrayOfFields[i].sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, saveFile);
		fwrite(arrayOfFields[i].pointerToDinamicArrayOfAttackedPositionsInOneTurn, sizeof(int), arrayOfFields[i].sizeOfDinamicArrayOfAttackedPositions, saveFile);
	}
	fclose(saveFile);
	startPositionOfString.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	cout << "File was saved successfully. Press key to continue... ";
	getch();
}

void downloadFromFile(PlayingField &playerField, PlayingField &compField) {
	PlayingField *arrayOfFields[2] = { &playerField, &compField };
	COORD startPositionOfString = fileNameEnterPosition;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	char *fileName = getNameOfFileFromConsole("Enter direction to file from which you want to load: ");
	FILE *loadFile = fopen(fileName, "rb");
	delete[] fileName;
	startPositionOfString.Y++;
	SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
	if (!loadFile) {
		cout << "Error: File was not founed.";
		startPositionOfString.Y++;
		SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
		cout << "Exiting from loading.";
		startPositionOfString.Y++;
		SetConsoleCursorPosition(ThisConsole.ConsoleDescriptor, startPositionOfString);
		cout << "Press key to coninue game...";
		getch();
		return;
	}
	for (int i = 0; i < 2; i++) {
		delete_din_array(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositions);
		delete_din_array(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositionsOfWoundedShip);
		delete_din_array(arrayOfFields[i]->pointerToDinamicArrayOfAttackedPositionsInOneTurn);
		fread(arrayOfFields[i], sizeof(PlayingField), 1, loadFile);
		allocate_memory(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositions, arrayOfFields[i]->sizeOfDinamicArrayOfPossiblePositions);
		allocate_memory(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, arrayOfFields[i]->sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip);
		allocate_memory(arrayOfFields[i]->pointerToDinamicArrayOfAttackedPositionsInOneTurn, arrayOfFields[i]->sizeOfDinamicArrayOfAttackedPositions);
		fread(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositions, sizeof(int), arrayOfFields[i]->sizeOfDinamicArrayOfPossiblePositions, loadFile);
		fread(arrayOfFields[i]->pointerToDinamicArrayOfPossiblePositionsOfWoundedShip, sizeof(int), arrayOfFields[i]->sizeOfDinamicArrayOfPossiblePositionsOfWoundedShip, loadFile);
		fread(arrayOfFields[i]->pointerToDinamicArrayOfAttackedPositionsInOneTurn, sizeof(int), arrayOfFields[i]->sizeOfDinamicArrayOfAttackedPositions, loadFile);
		initializeArrayOfShips(*arrayOfFields[i]);
	}
	fclose(loadFile);
	cout << "File was loaded successfully. Press key to continue... ";
	getch();
}

void initializeArrayOfShips(PlayingField &Field) {
	Field.shipArray[0] = &Field.FourSize;
	for (int i = 0; i < 2; i++) {
		Field.shipArray[1 + i] = Field.ThreeSize + i;
	}
	for (int i = 0; i < 3; i++) {
		Field.shipArray[3 + i] = Field.TwoSize + i;
	}
	for (int i = 0; i < 4; i++) {
		Field.shipArray[6 + i] = Field.OneSize + i;
	}
}

char *getNameOfFileFromConsole(const char* message) {
	char *fileName = new char[50];
	cout << message;
	cin >> fileName;
	return fileName;
}

template <typename T> void add_element_in_dinamic_array(T *&current_array, int &current_ar_size, T element) {
	T *temp_array = nullptr;
	allocate_memory(temp_array, current_ar_size + 1);
	copy_elements(temp_array, current_array, current_ar_size);
	temp_array[current_ar_size++] = element;
	delete_din_array(current_array);
	current_array = temp_array;
}

template <typename T> void delete_element_from_dinamic_array(T *&currentArray, int  &currentArSize, int index, const char *errorMessage) {
	index--;
	if (!checkIndex(index, currentArSize, errorMessage)) {
		return;
	}
	if (deleteOneSizedArray(currentArray, currentArSize)) {
		return;
	}
	T *temp_array = nullptr;
	allocate_memory(temp_array, --currentArSize);
	copy_elements(temp_array, currentArray, index);
	copy_elements((temp_array + index), (currentArray + index + 1), (currentArSize - index));
	delete[] currentArray;
	currentArray = temp_array;
}

bool checkIndex(int index, int currentArSize, const char *errorMessage) {
	if (index < 0 || index >= currentArSize) {
		cout << errorMessage;
		return false;
	}
	return true;
}
template <typename T> bool deleteOneSizedArray(T *&currentArray, int &currentArSize) {
	if (currentArSize == 1) {
		delete[] currentArray;
		currentArSize--;
		currentArray = nullptr;
		return true;
	}
	return false;
}
template <typename T> void allocate_memory(T *&Pointer, int size) {
	Pointer = new T[size];
}

template <typename T> void copy_elements(T *destination, T *source, int size_to_copy) {
	for (int i = 0; i < size_to_copy; i++) {
		destination[i] = source[i];
	}
}

template <typename T> void delete_din_array(T *&din_array) {
	if (din_array) {
		delete[] din_array;
		din_array = nullptr;
	}
}




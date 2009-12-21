#pragma once

#define TotalDeviceCount 3

// Доступные типы эмуляторов работы процесса
enum EmulatorType { EmulatorType_Predevice, EmulatorType_Indevice,
	EmulatorType_Postdevice };

// Доступные варианты результата работы процесса в течении выделенного
// интервала времени.
enum Result { Result_Sleep, Result_Run, Result_Wait, Result_Finish };

// Абстрактный класс эмулятора работы процесса
class BaseEmulator;

// Класс задаёт шаблон работы одного процесса
class ProcessOptions {
public:
	ProcessOptions(int PredeviceTicks, int IndeviceTicks, int WaitDeviceTicks,
		int DeviceCount, int PostdeviceTicks, int TotalCount, int Priority);
	int PredeviceTicks;  // Время выполнения до захвата устройства
	int IndeviceTicks;   // Время выполнения работы с устройством
	int WaitDeviceTicks; // Время ожидания формирования результата устройством
	int DeviceCount;     // Кол-во повторений работы с устройством
	int PostdeviceTicks; // Время выполнения после захвата устройства
	int TotalCount;      // Кол-во полных повторений работы процесса
	int Priority;        // Приоритет процесса
};

// Класс эмулирует работу одного процесса по заданному шаблону
class Process {
public:
	Process(ProcessOptions* Options); // Конструктор класса

protected:
	ProcessOptions* m_Options; // Шаблон работы процессора
	int m_CurrentPriority;     // Текущий приоритет
	int InstateTicks;          // Отработано тактов в текущем состоянии
	int TotalStep;             // Текущий шаг общий
	BaseEmulator* m_Emulator;  // Текущий эмулятор работы процесса
	int m_FinishTick;          // Время, на котором работа была окончена
	bool m_NoPrint;            // Блокировка вывода статистики
	Result m_LastResult;       // Последний результат работы процесса

public:
	Result DoTick(int TotalTick); // Отработать текущий шаг
	void PrintState();            // Вывести состояние в последнем вызове
	void PrintPriority();         // Вывести приоритет в последнем вызове
	static void PrintSemafore();  // Вывести кол-во свободных устройств
	int GetFinishTick();          // Время, на котором работа была окончена
	int GetPriority();            // Приоритет процесса по шаблону
	Result GetResult();           // Последний результат работы процесса
};

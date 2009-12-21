#pragma once

// Класс эмулирует менеджер управления процессами
class Manager {
private:
	int m_Step;          // Номер шага обработки всех процессов
	int m_TotalTick;     // Текущее виртуальное время эмулятора
	int m_Idle;          // Время простоя эмулируемого процессора
	int m_StepTicks;     // Время работы на текущем шаге
	int m_StepIdle;      // Время простоя на текущем шаге
	int m_ProcessCount;   // Кол-во эмулируемых процессов
	Process** m_Processes; // Ссылка на массив эмулируемых процессов

	// Форматирует текст о времени работы по числовым значениям
	void FormatTicks(char* Buffer, int Ticks, int Idle);
	// Выводит текст в центре области указанной длины
	void PrintIntAtCenter(int Value, int Length);

public:
	Manager(Process** Processes, int Count); // Конструктор класса
	bool RunStep();     // Запустить обработку очередного шага
	void PrintHeader(); // Вывод заголовка статистики
	void PrintStep();   // Вывод статистики по текущему шагу
	void PrintFooter(); // Вывод итогов статистики
};
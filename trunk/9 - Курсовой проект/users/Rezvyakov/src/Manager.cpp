#include "stdafx.h"
#include "Data.h"
#include "Manager.h"

// Конструктор класса
Manager::Manager(Process** Processes, int Count) {
	m_Step = 0;
	m_TotalTick = 0;
	m_Idle = 0;
	m_StepTicks = 0;
	m_StepIdle = 0;
	m_Processes = Processes;
	m_ProcessCount = Count;
}

// Запустить обработку очередного шага
bool Manager::RunStep() {
	int l_BeginTick = m_TotalTick;
	bool l_AllWait = true;
	bool l_AllFinish = true;
	for (int index = 0, count = m_ProcessCount; index < count; index++) {
		Process* l_Process = m_Processes[index];
		Result l_Result = l_Process->DoTick(m_TotalTick);
		if (l_Result == Result_Run) m_TotalTick++;
		if (l_Result != Result_Finish) {
			l_AllFinish = false;
			if (l_Result != Result_Wait) l_AllWait = false;
		}
	}
	m_Step++;
	m_StepIdle = 0;
	if ((m_StepTicks = m_TotalTick - l_BeginTick) == 0) {
		if (l_AllFinish) return false;
		if (l_AllWait) {
			m_TotalTick++;
			m_Idle++;
			m_StepIdle = 1;
		}
	}
	return true;
}

// Вывод заголовка статистики
void Manager::PrintHeader() {
	char l_Buffer[32];
	int count = m_ProcessCount;

	printf("processes:");
	strcpy(l_Buffer, "  x   ");
	for (int index = 0; index < count; index++) {
		l_Buffer[2] = index + 'A';
		printf(l_Buffer);
	}
	printf("sem.time/i\npriority: ");

	strcpy(&l_Buffer[0],  " high ");
	strcpy(&l_Buffer[8],  " norm ");
	strcpy(&l_Buffer[16], " low  ");
	strcpy(&l_Buffer[24], " (x)  ");
	for (int index = 0; index < count; index++) {
		int l_Priority;
		switch (l_Priority = m_Processes[index]->GetPriority()) {
			case 1:  printf(&l_Buffer[0]); break;
			case 2:  printf(&l_Buffer[8]); break;
			case 3:  printf(&l_Buffer[16]); break;
			default: l_Buffer[26] = l_Priority + '0'; printf(&l_Buffer[24]);
				break;
		}
	}
	printf("\n");
}

// Вывод статистики по текущему шагу
void Manager::PrintStep() {
	printf("\n");

	char buffer[11];
	itoa(m_Step, buffer, 10);
	int lenght = strlen(buffer);
	memcpy(buffer + 8 - lenght, buffer, lenght);
	memcpy(buffer, "step    ", 8 - lenght);
	strcpy(buffer + 8, ": ");
	printf(buffer);

	int count = m_ProcessCount;
	for (int index = 0; index < count; index++)
		m_Processes[index]->PrintPriority();

	Process::PrintSemafore();
	FormatTicks(buffer, m_StepTicks, m_StepIdle);
	buffer[0] = '+';
	printf(buffer);

	printf("\n          ");
	for (int index = 0; index < count; index++)
		m_Processes[index]->PrintState();

	FormatTicks(buffer, m_TotalTick - m_Idle, m_Idle);
	printf("   =%s\n", buffer);
}

// Форматирует текст о времени работы по числовым значениям
void Manager::FormatTicks(char* Buffer, int Ticks, int Idle) {
	itoa(Ticks, Buffer, 10);
	int lenght = strlen(Buffer);
	if (lenght < 3) {
		memcpy(Buffer + 3 - lenght, Buffer, lenght);
		memcpy(Buffer, "  ", 3 - lenght);
	}
	Buffer[3] = '/';
	itoa(Idle, Buffer + 4, 10);
	if (Buffer[5]) Buffer[6] = 0; else strcpy(Buffer + 5, " ");
}

// Вывод итогов статистики
void Manager::PrintFooter() {
	printf("----------");
	for (int index = m_ProcessCount; index; index--)
		printf("------");
	printf("----------\n");

	printf("TotalTime:");
	for (int index = 0, count = m_ProcessCount; index < count; index++)
		PrintIntAtCenter(m_Processes[index]->GetFinishTick(), 6);
	printf("   =");
	PrintIntAtCenter(m_TotalTick, 6);
	printf("\n");
}

// Выводит текст в центре области указанной длины
void Manager::PrintIntAtCenter(int Value, int Length) {
	char* buffer = new char[Length + 1];
	itoa(Value, buffer, 10);
	int length = strlen(buffer);
	int begin = (Length - length) / 2;
	if (begin) {
		memcpy(buffer + begin, buffer, length);
		memset(buffer, ' ', begin);
	}
	memset(buffer + begin + length, ' ', Length - begin - length);
	buffer[Length] = 0;
	printf(buffer);
}
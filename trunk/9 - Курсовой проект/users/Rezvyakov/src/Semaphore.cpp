#include "stdafx.h"
#include "Data.h"
#include "Manager.h"

#define ProcessesCount 7

// Инициализация менеджера процессов, задание параметров эмуляции
Manager* Initialize() {
	// Задание шаблонов работы процессов
	ProcessOptions* l_OptionsH = new ProcessOptions(2, 1, 2, 3, 1, 2, 1);
	ProcessOptions* l_OptionsN = new ProcessOptions(2, 1, 2, 3, 1, 2, 2);
	ProcessOptions* l_OptionsL = new ProcessOptions(2, 1, 2, 3, 1, 2, 3);

	Process** l_Processes = new Process*[ProcessesCount];
	l_Processes[0] = new Process(l_OptionsH);
	l_Processes[1] = new Process(l_OptionsH);
	l_Processes[2] = new Process(l_OptionsN);
	l_Processes[3] = new Process(l_OptionsN);
	l_Processes[4] = new Process(l_OptionsN);
	l_Processes[5] = new Process(l_OptionsL);
	l_Processes[6] = new Process(l_OptionsL);
	return new Manager(l_Processes, ProcessesCount);
}

// Главная функция эмулятора
int _tmain(int argc, _TCHAR* argv[]) {
	Manager* l_Manager = Initialize();
	l_Manager->PrintHeader();

	bool result;
	do {
		result = l_Manager->RunStep();
		l_Manager->PrintStep();
	} while (result);
	l_Manager->PrintFooter();
	delete l_Manager;
	scanf("\n");
	return 0;
}
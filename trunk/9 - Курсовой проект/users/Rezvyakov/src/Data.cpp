#include "stdafx.h"
#include "Data.h"

// Класс эмулирующий работу семафора
class Devices {
public:
	// Конструктор класса
	Devices(int DeviceCount) {
		this->CurrentCount = DeviceCount;
	}

	// Текущее кол-во свободных устройств
	int CurrentCount;

	// Попытаться занять устройство
	bool GetDevice() {
		if (this->CurrentCount) {
			this->CurrentCount--;
			return true;
		} else
			return false;
	}

	// Освободить устройство
	void ReleaseDevice()
	{ this->CurrentCount++; }
};

// Устройства, эмулятор семафора
Devices* g_Devices = new Devices(TotalDeviceCount);

// Базовый абстрактный класс эмулятора работы процесса
class BaseEmulator abstract {
public:
	Result virtual DoTick(int TotalTick) = 0; // Отработать текущий шаг
	bool virtual IsFinished() = 0;            // Эмулятор закончил работу?
	EmulatorType virtual GetType() = 0;       // Тип класса эмулятора
	void virtual PrintState() = 0;            // Вывод текущего состояния
};

// Базовый класс эмулятор работы процесса до и после работы с устройством
class BaseNodeviceEmulator abstract : public BaseEmulator
{
protected:
	int m_TicksToWork; // Время, которое осталось обрабатывать устройство

public:
	// Отработать текущий шаг
	Result DoTick(int TotalTick) {
		m_TicksToWork--;
		return Result_Run;
	}

	// Эмулятор закончил эмуляцию своей части шаблона работы процесса?
	bool IsFinished() { return !m_TicksToWork; }
	// Вывод текущего состояния работы процесса
	void PrintState() { printf(" run  "); }
};

// Эмулятор работы процесса до работы с устройством
class PredeviceEmulator : public BaseNodeviceEmulator {
public:
	// Конструктор класса
	PredeviceEmulator(ProcessOptions* Options)
	{ this->m_TicksToWork = Options->PredeviceTicks; }
	// Тип класса
	EmulatorType GetType() { return EmulatorType_Predevice; }
};

// Эмулятор работы процесса после работы с устройством
class PostdeviceEmulator : public BaseNodeviceEmulator {
public:
	// Конструктор класса
	PostdeviceEmulator(ProcessOptions* Options)
	{ this->m_TicksToWork = Options->PostdeviceTicks; }
	// Тип класса
	EmulatorType GetType() { return EmulatorType_Postdevice; }
};

// Эмулятор работы процесса с устройством
class IndeviceEmilator : public BaseEmulator {
public:
	// Конструктор класса
	IndeviceEmilator(ProcessOptions* Options) {
		this->m_Options = Options;
		this->m_DeviceStep = (Options->DeviceCount
			&& (Options->IndeviceTicks || Options->WaitDeviceTicks)) ? -1 : 0;
		this->m_SleepToTick = 0;
		this->m_TicksToWork = Options->IndeviceTicks;
	}

protected:
	ProcessOptions* m_Options; // Ссылка на шаблон работы процесса
	int m_DeviceStep;    // Текущий шаг в устройстве
	int m_SleepToTick;   // Ожидание до указанного здесь времени
	int m_TicksToWork;   // Время, которое осталось обрабатывать устройство
	Result m_LastResult; // Последний результат эмулятора

public:
	// Отработать текущий шаг
	Result DoTick(int TotalTick) {
		if (m_DeviceStep == -1) {
			if (!g_Devices->GetDevice()) return m_LastResult = Result_Wait;
			m_DeviceStep = m_Options->DeviceCount;
		}
		while (m_DeviceStep) {
			if (m_SleepToTick) { // Режим ожидания устройства
				if (m_SleepToTick <= TotalTick) { // Ожидание окончено
					m_SleepToTick = 0;
					m_TicksToWork = m_Options->IndeviceTicks;
					m_DeviceStep--;
					continue;
				} else return m_LastResult = Result_Wait; // Нужно ждать дальше
			} else { // Режим работы
				bool l_Runned = false;
				if (m_TicksToWork) { // Если работа не окончена
					if (--m_TicksToWork) return m_LastResult = Result_Run;
					l_Runned = true; // Если работа выполнена на текущем шаге,
						// то следует вычислить такт окончания ожидания
				}
				m_SleepToTick = TotalTick + m_Options->WaitDeviceTicks;
				if (l_Runned) {
					m_SleepToTick++; return m_LastResult = Result_Run; }
				else continue;
			}
		}
		g_Devices->ReleaseDevice();
		return m_LastResult = Result_Finish;
	}
	// Эмулятор закончил эмуляцию своей части шаблона работы процесса?
	bool IsFinished() { return !m_DeviceStep; }
	// Процесс находится в ожидании устройства (освобождения или ответа)?
	bool IsWaitOrSleep() { return m_LastResult == Result_Wait; }
	// Тип класса
	EmulatorType GetType() { return EmulatorType_Indevice; }
	// Вывод текущего состояния работы процесса
	void PrintState() { printf(m_DeviceStep == -1
		? " wait " : m_LastResult == Result_Run ? "d:run " : "d:slp "); }
};

/*******************************************************************************
**************                  ProcessOptions                    **************
*******************************************************************************/

ProcessOptions::ProcessOptions(int PredeviceTicks, int IndeviceTicks,
		int WaitDeviceTicks, int DeviceCount, int PostdeviceTicks,
		int TotalCount, int Priority) {
	this->PredeviceTicks  = PredeviceTicks;
	this->IndeviceTicks   = IndeviceTicks;
	this->WaitDeviceTicks = WaitDeviceTicks;
	this->DeviceCount     = DeviceCount;
	this->PostdeviceTicks = PostdeviceTicks;
	this->TotalCount      = TotalCount;
	this->Priority        = Priority;
}

/*******************************************************************************
**************                     Process                        **************
*******************************************************************************/

// Конструктор класса
Process::Process(ProcessOptions* Options) {
	this->m_Options = Options;
	m_CurrentPriority = Options->Priority;
	this->TotalStep = Options->TotalCount;
	this->m_Emulator = this->TotalStep ? new PredeviceEmulator(Options) : 0;
	this->m_FinishTick = 0;
	this->m_NoPrint = false;
}

// Отработать текущий шаг
Result Process::DoTick(int TotalTick) {
	bool l_Sleep = false;
	if (!m_Emulator) return m_LastResult = Result_Finish;
	if ((m_Emulator->GetType() == EmulatorType_Indevice
				&& ((IndeviceEmilator*)m_Emulator)->IsWaitOrSleep())
			|| !--m_CurrentPriority) m_CurrentPriority = m_Options->Priority;
	else l_Sleep = true;

	Result result;
	do {
		while (m_Emulator && m_Emulator->IsFinished())
			switch (m_Emulator->GetType()) {
				case EmulatorType_Predevice:
					delete m_Emulator;
					m_Emulator = new IndeviceEmilator(m_Options);
					break;
				case EmulatorType_Indevice:
					delete m_Emulator;
					m_Emulator = new PostdeviceEmulator(m_Options);
					break;
				case EmulatorType_Postdevice:
					delete m_Emulator;
					if (!--TotalStep) {
						m_Emulator = 0; m_FinishTick = TotalTick;
						return m_LastResult = Result_Finish; }
					m_Emulator = new PredeviceEmulator(m_Options);
					break;
			}
		if (l_Sleep) return m_LastResult = Result_Sleep;
		result = m_Emulator->DoTick(TotalTick);
	} while (result == Result_Finish);
	return m_LastResult = result;
}

// Вывести состояние в последнем вызове
void Process::PrintState() {
	if (!m_Emulator) printf(m_NoPrint || !(m_NoPrint = true) ? "      " : " end  ");
	else if (m_Options->Priority == m_CurrentPriority) m_Emulator->PrintState();
	else printf("sleep ");
}

// Вывести приоритет в последнем вызове
void Process::PrintPriority() {
	if (!m_Emulator) printf(m_NoPrint ? "      " : " ---  ");
	else if (m_Emulator && m_Emulator->GetType() == EmulatorType_Indevice
			&& ((IndeviceEmilator*)m_Emulator)->IsWaitOrSleep()) printf("  -   ");
	else if (m_Options->Priority == m_CurrentPriority) printf(" 1>0  ");
	else {
		char buffer[7];
		strcpy(buffer, " ");
		itoa(m_CurrentPriority + 1, buffer + 1, 36);
		strcpy(buffer + 2, ">");
		itoa(m_CurrentPriority, buffer + 3, 36);
		strcpy(buffer + 4, "  ");
		printf(buffer);
	}
}

// Вывести кол-во свободных устройств
void Process::PrintSemafore() {
	char buffer[5];
	itoa(g_Devices->CurrentCount, buffer + 1, 36);
	buffer[0] = '-';
	strcpy(buffer + 2, "- ");
	printf(buffer);
}

// Время, на котором работа была окончена
int Process::GetFinishTick() { return m_FinishTick; }
// Приоритет процесса по шаблону
int Process::GetPriority() { return m_Options->Priority; }
// Последний результат работы процесса
Result Process::GetResult() { return m_LastResult; }
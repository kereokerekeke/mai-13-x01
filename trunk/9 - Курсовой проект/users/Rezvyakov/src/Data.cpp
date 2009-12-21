#include "stdafx.h"
#include "Data.h"

// ����� ����������� ������ ��������
class Devices {
public:
	// ����������� ������
	Devices(int DeviceCount) {
		this->CurrentCount = DeviceCount;
	}

	// ������� ���-�� ��������� ���������
	int CurrentCount;

	// ���������� ������ ����������
	bool GetDevice() {
		if (this->CurrentCount) {
			this->CurrentCount--;
			return true;
		} else
			return false;
	}

	// ���������� ����������
	void ReleaseDevice()
	{ this->CurrentCount++; }
};

// ����������, �������� ��������
Devices* g_Devices = new Devices(TotalDeviceCount);

// ������� ����������� ����� ��������� ������ ��������
class BaseEmulator abstract {
public:
	Result virtual DoTick(int TotalTick) = 0; // ���������� ������� ���
	bool virtual IsFinished() = 0;            // �������� �������� ������?
	EmulatorType virtual GetType() = 0;       // ��� ������ ���������
	void virtual PrintState() = 0;            // ����� �������� ���������
};

// ������� ����� �������� ������ �������� �� � ����� ������ � �����������
class BaseNodeviceEmulator abstract : public BaseEmulator
{
protected:
	int m_TicksToWork; // �����, ������� �������� ������������ ����������

public:
	// ���������� ������� ���
	Result DoTick(int TotalTick) {
		m_TicksToWork--;
		return Result_Run;
	}

	// �������� �������� �������� ����� ����� ������� ������ ��������?
	bool IsFinished() { return !m_TicksToWork; }
	// ����� �������� ��������� ������ ��������
	void PrintState() { printf(" run  "); }
};

// �������� ������ �������� �� ������ � �����������
class PredeviceEmulator : public BaseNodeviceEmulator {
public:
	// ����������� ������
	PredeviceEmulator(ProcessOptions* Options)
	{ this->m_TicksToWork = Options->PredeviceTicks; }
	// ��� ������
	EmulatorType GetType() { return EmulatorType_Predevice; }
};

// �������� ������ �������� ����� ������ � �����������
class PostdeviceEmulator : public BaseNodeviceEmulator {
public:
	// ����������� ������
	PostdeviceEmulator(ProcessOptions* Options)
	{ this->m_TicksToWork = Options->PostdeviceTicks; }
	// ��� ������
	EmulatorType GetType() { return EmulatorType_Postdevice; }
};

// �������� ������ �������� � �����������
class IndeviceEmilator : public BaseEmulator {
public:
	// ����������� ������
	IndeviceEmilator(ProcessOptions* Options) {
		this->m_Options = Options;
		this->m_DeviceStep = (Options->DeviceCount
			&& (Options->IndeviceTicks || Options->WaitDeviceTicks)) ? -1 : 0;
		this->m_SleepToTick = 0;
		this->m_TicksToWork = Options->IndeviceTicks;
	}

protected:
	ProcessOptions* m_Options; // ������ �� ������ ������ ��������
	int m_DeviceStep;    // ������� ��� � ����������
	int m_SleepToTick;   // �������� �� ���������� ����� �������
	int m_TicksToWork;   // �����, ������� �������� ������������ ����������
	Result m_LastResult; // ��������� ��������� ���������

public:
	// ���������� ������� ���
	Result DoTick(int TotalTick) {
		if (m_DeviceStep == -1) {
			if (!g_Devices->GetDevice()) return m_LastResult = Result_Wait;
			m_DeviceStep = m_Options->DeviceCount;
		}
		while (m_DeviceStep) {
			if (m_SleepToTick) { // ����� �������� ����������
				if (m_SleepToTick <= TotalTick) { // �������� ��������
					m_SleepToTick = 0;
					m_TicksToWork = m_Options->IndeviceTicks;
					m_DeviceStep--;
					continue;
				} else return m_LastResult = Result_Wait; // ����� ����� ������
			} else { // ����� ������
				bool l_Runned = false;
				if (m_TicksToWork) { // ���� ������ �� ��������
					if (--m_TicksToWork) return m_LastResult = Result_Run;
					l_Runned = true; // ���� ������ ��������� �� ������� ����,
						// �� ������� ��������� ���� ��������� ��������
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
	// �������� �������� �������� ����� ����� ������� ������ ��������?
	bool IsFinished() { return !m_DeviceStep; }
	// ������� ��������� � �������� ���������� (������������ ��� ������)?
	bool IsWaitOrSleep() { return m_LastResult == Result_Wait; }
	// ��� ������
	EmulatorType GetType() { return EmulatorType_Indevice; }
	// ����� �������� ��������� ������ ��������
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

// ����������� ������
Process::Process(ProcessOptions* Options) {
	this->m_Options = Options;
	m_CurrentPriority = Options->Priority;
	this->TotalStep = Options->TotalCount;
	this->m_Emulator = this->TotalStep ? new PredeviceEmulator(Options) : 0;
	this->m_FinishTick = 0;
	this->m_NoPrint = false;
}

// ���������� ������� ���
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

// ������� ��������� � ��������� ������
void Process::PrintState() {
	if (!m_Emulator) printf(m_NoPrint || !(m_NoPrint = true) ? "      " : " end  ");
	else if (m_Options->Priority == m_CurrentPriority) m_Emulator->PrintState();
	else printf("sleep ");
}

// ������� ��������� � ��������� ������
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

// ������� ���-�� ��������� ���������
void Process::PrintSemafore() {
	char buffer[5];
	itoa(g_Devices->CurrentCount, buffer + 1, 36);
	buffer[0] = '-';
	strcpy(buffer + 2, "- ");
	printf(buffer);
}

// �����, �� ������� ������ ���� ��������
int Process::GetFinishTick() { return m_FinishTick; }
// ��������� �������� �� �������
int Process::GetPriority() { return m_Options->Priority; }
// ��������� ��������� ������ ��������
Result Process::GetResult() { return m_LastResult; }
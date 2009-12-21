#pragma once

#define TotalDeviceCount 3

// ��������� ���� ���������� ������ ��������
enum EmulatorType { EmulatorType_Predevice, EmulatorType_Indevice,
	EmulatorType_Postdevice };

// ��������� �������� ���������� ������ �������� � ������� �����������
// ��������� �������.
enum Result { Result_Sleep, Result_Run, Result_Wait, Result_Finish };

// ����������� ����� ��������� ������ ��������
class BaseEmulator;

// ����� ����� ������ ������ ������ ��������
class ProcessOptions {
public:
	ProcessOptions(int PredeviceTicks, int IndeviceTicks, int WaitDeviceTicks,
		int DeviceCount, int PostdeviceTicks, int TotalCount, int Priority);
	int PredeviceTicks;  // ����� ���������� �� ������� ����������
	int IndeviceTicks;   // ����� ���������� ������ � �����������
	int WaitDeviceTicks; // ����� �������� ������������ ���������� �����������
	int DeviceCount;     // ���-�� ���������� ������ � �����������
	int PostdeviceTicks; // ����� ���������� ����� ������� ����������
	int TotalCount;      // ���-�� ������ ���������� ������ ��������
	int Priority;        // ��������� ��������
};

// ����� ��������� ������ ������ �������� �� ��������� �������
class Process {
public:
	Process(ProcessOptions* Options); // ����������� ������

protected:
	ProcessOptions* m_Options; // ������ ������ ����������
	int m_CurrentPriority;     // ������� ���������
	int InstateTicks;          // ���������� ������ � ������� ���������
	int TotalStep;             // ������� ��� �����
	BaseEmulator* m_Emulator;  // ������� �������� ������ ��������
	int m_FinishTick;          // �����, �� ������� ������ ���� ��������
	bool m_NoPrint;            // ���������� ������ ����������
	Result m_LastResult;       // ��������� ��������� ������ ��������

public:
	Result DoTick(int TotalTick); // ���������� ������� ���
	void PrintState();            // ������� ��������� � ��������� ������
	void PrintPriority();         // ������� ��������� � ��������� ������
	static void PrintSemafore();  // ������� ���-�� ��������� ���������
	int GetFinishTick();          // �����, �� ������� ������ ���� ��������
	int GetPriority();            // ��������� �������� �� �������
	Result GetResult();           // ��������� ��������� ������ ��������
};

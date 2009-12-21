#pragma once

// ����� ��������� �������� ���������� ����������
class Manager {
private:
	int m_Step;          // ����� ���� ��������� ���� ���������
	int m_TotalTick;     // ������� ����������� ����� ���������
	int m_Idle;          // ����� ������� ������������ ����������
	int m_StepTicks;     // ����� ������ �� ������� ����
	int m_StepIdle;      // ����� ������� �� ������� ����
	int m_ProcessCount;   // ���-�� ����������� ���������
	Process** m_Processes; // ������ �� ������ ����������� ���������

	// ����������� ����� � ������� ������ �� �������� ���������
	void FormatTicks(char* Buffer, int Ticks, int Idle);
	// ������� ����� � ������ ������� ��������� �����
	void PrintIntAtCenter(int Value, int Length);

public:
	Manager(Process** Processes, int Count); // ����������� ������
	bool RunStep();     // ��������� ��������� ���������� ����
	void PrintHeader(); // ����� ��������� ����������
	void PrintStep();   // ����� ���������� �� �������� ����
	void PrintFooter(); // ����� ������ ����������
};
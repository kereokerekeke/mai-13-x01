program Laba1;

var
  a, b, c, d, r1, r2: integer;

begin
  a :=  29;
  b :=  18;
  c :=  31;
  d := -14;

  asm
    mov  AX, c  {�������� �������� C � ������� AX}
    imul d      {�������� C (� AX) �� D}
    mov  r2, AX {�������� ��������� �� AX � ���������� r2}

    mov  AX, a  {�������� �������� A � ������� AX}
    cwd         {��������� �������� � AX �� (DX:AX), �� �������� �����}
    idiv b      {����� (DX:AX) �� �������� B}
    add  r2, DX {��������� ������� �� ������� (�� DX) � ���������� r2}

    mov  AX, a  {�������� �������� A � ������� AX}
    imul AX     {�������� A (� AX) ���� �� ����}
    sub  AX, 7  {�������� �� ������������ 7}
    add  r2, AX {��������� �������� (�� AX) � ���������� r2}
  end;

  r1 := c*d + (a mod b) + (a*a - 7);
  WriteLn('Result by Pascal: ', r1:4);
  WriteLn('Result by Assembler: ', r2:4);
  ReadLn;
end.
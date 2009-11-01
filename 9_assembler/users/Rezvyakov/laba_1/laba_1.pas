program Laba1;

var
  a, b, c, d, r1, r2: integer;

begin
  a :=  29;
  b :=  18;
  c :=  31;
  d := -14;

  asm
    mov  AX, c  {помещаем значение C в регистр AX}
    imul d      {умножаем C (в AX) на D}
    mov  r2, AX {копируем результат из AX в переменную r2}

    mov  AX, a  {копируем значение A в регистр AX}
    cwd         {расширяем значение в AX до (DX:AX), до двойного слова}
    idiv b      {делим (DX:AX) на значение B}
    add  r2, DX {добавляем остаток от деления (из DX) в переменную r2}

    mov  AX, a  {копируем значение A в регистр AX}
    imul AX     {умножаем A (в AX) само на себя}
    sub  AX, 7  {вычитаем из произведения 7}
    add  r2, AX {добавляем разность (из AX) в переменную r2}
  end;

  r1 := c*d + (a mod b) + (a*a - 7);
  WriteLn('Result by Pascal: ', r1:4);
  WriteLn('Result by Assembler: ', r2:4);
  ReadLn;
end.
    sseg segment stack
         db 64 dup (?)
    sseg ends
         ;
    dseg segment
         str_result db "Result: $"
         value_a dw  29
         value_b dw  18
         value_c dw  31
         value_d dw -14
    dseg ends
         ;
    cseg segment
         assume cs:cseg, ds:dseg, ss:sseg
         ;
   start proc far
         push ds
         push ax
         mov  bx, dseg
         mov  ds, bx

         call outtxt
         call calc
         call outint

         ret
   start endp
         ;
         ; ��������� ������ ������ "Result: ". ��������� ������ AH, DX.
  outtxt proc near
         lea  dx, str_result ; ���������� - "mov dx, offset str_result"
         mov  ah, 9
         int  21h
         ret
  outtxt endp
         ;
         ; ��������� ���������� �������: c*d + (a mod b) + (a^2 - 7).
         ; ��������� ������ AX, CX, DX.
    calc proc near
         mov  ax, value_c ; ������� �������� C � ������� AX
         imul value_d     ; �������� AX �� �������� D
         mov  cx, ax      ; �������� ��������� � ������� CX

         mov  ax, value_a ; ������� �������� A � ������� AX
         cwd              ; ��������� �������� AX �� (DX,AX)
         idiv value_b     ; ����� (DX,AX) �� �������� B
         add  cx, dx      ; ���������� ������� �� ������� � ����� � CX
      
         mov  ax, value_a ; ������� �������� A � ������� AX
         imul ax          ; �������� AX ���� �� ����
         sub  ax, 7       ; �������� �� ���������� (AX) ����� 7
         add  ax, cx      ; ���������� CX � AX, ��������� ������� � AX
         ret
    calc endp
         ;
         ; ��������� ������ �����, ����������� � AX.
         ; ��������� ������ AX, BX, CX, DX.
  outint proc near
         xor  bx, bx  ; �������� BX - ��� ����� ������� �������������� �����
         mov  cx, 10  ; � CX ������� ����� 10 - ��������� ���������� �������
         push bx      ; ������� ���� � ���� - ��� ������� ����� ������
         cmp  ax, 0   ; ���������� ��������� ����� � ����
         jnl  oi_get  ; ...���� �� �������������, �� ��������� � �����������
         inc  bl      ; ������������� ������� �������������� �����
         neg  ax      ; ���� ����� ������ �������������
 oi_get: xor  dx, dx  ; �������� ������� DX ����� ��������, �.�. AX >= 0
         idiv cx      ; ����� (DX,AX) �� CX, �.�. �� 10
         or   dl, 30h ; ����������� ����� � ��� �������, �������� ��� 30h
         push dx      ; ������� ��������� ������ � ����
         cmp  ax, 0   ; ���������� ��������� ������� � ����
         jne  oi_get  ; ...���� ��������� �� ����, �� ���������� ������
         
         mov  ah, 6   ; ������� ��� 6 ��� ������� ������ ������� �� �����
         cmp  bl, 0   ; ���������� ������� �������������� ����� � ����
         je   oi_put  ; ...���� �������� ���, �� ��������� � ������ �����
         mov  dx, '-' ; �������� ��� ������� ����� � �������
 oi_nxt: int  21h     ; �������� ��������� ������� 6 - ������ �������
 oi_put: pop  dx      ; ����������� ��������� ��� ������� �� �����
         cmp  dx, 0   ; ���������� ������ � ��������� ����� ������ - � ����
         jne  oi_nxt  ; ...���� ������ �� �������, �� ���������� ����
 oi_out: ret
  outint endp
  
    cseg ends
         end start
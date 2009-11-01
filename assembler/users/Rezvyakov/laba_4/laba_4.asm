    sseg segment stack
         db 64 dup (?)
    sseg ends
         ;
    dseg segment
         str_result db "f(x) = result", 10, 13, " -----------", 10, 13, "$"
         str_bgn    db "f($"
         str_mdl    db ") = $"
         str_end    db 10, 13, "$"
         value_a dw -6
         value_b dw  4
         value_c dw  8
         value_d dw  2
         x_from  dw -2
         x_to    dw  5
    dseg ends
         ;
    cseg segment
         assume cs:cseg, ds:dseg, ss:sseg
         ;
         ; ��������� ���������� � ������ ���������� �������
         ;   ��� ������ �������� X �� ���������� [x_from, x_to].
         ; ��������� ������ AX, BX, CX, DX.
 foreach proc near
         mov  cx, x_to    ; ��������� � ������� CX ����� ���������� �������� X
         inc  cx          ; ���������� CX �� 1 ��� �������������� �������� � X
         push bp          ; ���������� ������� ���������� �������� �����
         push cx          ; ���������� �������� ��� �������� ������ �������
         mov  bp, sp      ; ��������� ���������� �������� ����� �� ��������
         sub  cx, x_from  ; ��������� �� CX ���������� X - ���-�� ����� �����

 fe_bgn: lea  dx, str_bgn ; �������� � ������� DX ����� ������-��������
         mov  ah, 9       ; �������� � ������� AH ��� �������� ������ ������
         int  21h         ; �������� ���������� ��� ������ ������ �� �����
         mov  ax, [ss:bp] ; ��������� �������� �������� ������ �������
         sub  ax, cx      ; �������� ������� ��� � �������� ������� X.
         push cx          ; ����� CX (������� ����� �����) � ����
         push ax          ; ����� AX (������� �������� X) � ����
         call outint      ; ������� ������� �������� ����� X (� �������� AX)
         lea  dx, str_mdl ; �������� � ������� DX ����� ������-�����������
         mov  ah, 9       ; �������� � ������� AH ��� �������� ������ ������
         int  21h         ; �������� ���������� ��� ������ ������ �� �����
         mov  ax, value_a ; ����� � ������� AX �������� A (��� ������ ������)
         pop  bx          ; ��������������� �������� X �� ����� � ������� BX
         cmp  bx, 0       ; ���������� ������� X (� BX) � ����
         jl   fe_neg      ; ���� X ������ ����, ��������� �� �������...
         je   fe_zro      ; ���� X ����� ����, ��������� �� �������...
 fe_pos: imul ax          ; X �������������. �������� AX ���� �� ����
         sub  ax, bx      ; �������� X (� BX): (DX,AX) = (DX,AX) - BX
         sbb  dx, 0       ; ���������� ��������� ��� DX (A^2 - (0:X))
         mov  cx, value_c ; ����� �������� C � ������� CX
         add  cx, value_d ; ���������� � C (� CX) �������� D - C+D
         idiv cx          ; �������� ���������: AX = DX:AX / �X
         jmp  fe_nxt      ; ��������� �� ����� ����������
 fe_zro: sub  ax, value_b ; X ����� ����. ��������: AX=A-B, � �������� AX
         cwd              ; ��������� ����� �� �������� ����� DX:AX = A-B
         idiv value_d     ; �������� ���������: AX = DX:AX / D
         jmp  fe_nxt      ; ��������� �� ����� ����������
 fe_neg: mov  cx, 2       ; X �������������. �������� � ������� CX ����� 2
         imul cx          ; �������� AX �� 2:  (DX:AX) = 2*A
         imul bx          ; �������� AX �� BX: (DX:AX) = 2*A*X
         add  ax, 5       ; �������� ���������: AX = 2*A*X + 5
 fe_nxt: call outint      ; ������� ������� ��������� �� ���������� X (� AX)
         lea  dx, str_end ; �������� � ������� DX ����� ����� ��� ����� ������
         mov  ah, 9       ; �������� � ������� AH ��� �������� ������ ������
         int  21h         ; �������� ���������� ��� ������ ������ �� �����
         pop  cx          ; ��������������� �������� �������� ����� �� �����
         loop fe_bgn      ; ����������� ����� ��� ��� ���������
         pop  cx          ; �������� �� ����� �������� ������ �������
         pop  bp          ; ��������������� ��������� ���������� �������� �����
         ret
 foreach endp
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
         ; ����� ����� �������, ������� ��� �� �����
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
         ;
   start proc far
         push ds
         push ax
         mov  bx, dseg
         mov  ds, bx

         lea  dx, str_result ; ���������� - "mov dx, offset str_result"
         mov  ah, 9
         int  21h
         call foreach
         ret
   start endp
         ;
    cseg ends
         end start
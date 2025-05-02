        .model small
        .586
        .stack 100h
        .data
_S0      DB      "Enter a number","$"
_S1      DB      "The answer is ","$"
        .code
        include io.asm

x     PROC
        push bp
        mov bp, sp
        sub sp, 12 ;;Size of Local variables
       
        mov dx, offset _S0
        call writestr
       
        call readint
        mov [bp-2], bx
       
        mov ax, 10
        mov [bp-4], ax
       
        mov ax, 20
        mov [bp-8], ax
        
        mov ax, [bp-2]
        mov bx, [bp-4]
        imul bx
        mov [bp-10], ax
        
        mov ax, [bp-8]
        add ax, [bp-10]
        mov [bp-12], ax
        
        mov ax, [bp-12]
        mov [bp-6], ax
        
        mov dx, offset _S1
        call writestr
        
        mov dx, [bp-6]
        call writeint
        call writeln
       
        add sp, 12  ;;Size of Locals
        pop bp
        ret 0       ;;Size of Parmas
x     endp
two      PROC
        push bp
        mov bp, sp
        sub sp, 0  ;;Size of Local variables
        call x
        add sp, 0
        pop bp
        ret 0 ;;Size of Params
two      endp

start   PROC
        mov ax, @data
        mov ds, ax
        call two
        mov ah, 4ch
        mov al,0
        int 21h
start ENDP
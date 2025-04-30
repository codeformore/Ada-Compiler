# Translation of Three Address Code (TAC) to Intel 8086 Assembly

This document provides a structured guide for translating Three Address Code (TAC) into Intel 8086 Assembly language. Each section corresponds to a specific TAC operation and describes the translation process.

---

## Table of Contents
1. [General Translation Rules](#general-translation-rules)
2. [TAC to Assembly Mappings](#tac-to-assembly-mappings)
    - [Program Template](#program-template)
    - [Binary Operations](#binary-operations)
    - [Unary Operations](#unary-operations)
    - [Procedures](#procedures)
    - [IO](#io)
3. [Examples](#examples)
4. [References](#references)

---

## General Operand Translation Rules

1. Stack Variable (``StackTAC``)
A stack variable will have an integer offset from the base pointer (See [Stack Frame](#stack-frame)). This will be translated to `[BP-$offset]` for value and `BP-$offset` for address.

2. Global Variable (``GlobalTAC``)
A global variable will be stored in the data segment with its lexeme as its label. Thus, the label is used in the assembly code.

3. Constants (``ConstTAC``)
Constants will be immediate values for instructions where they are used.

4. String Literal
**TBD**

---

## TAC to Assembly Mappings

### Program Template
The output program will use a small memory model with a stack size of 100h. The template is as follows:
```asm
.model  small
.586
.stack  100h
.data
; GLOBAL VARIABLES and STRING LITERALS HERE (Will be replaced by start proc func call)
.code
include io.asm
```

Then, the program will close when the final line of TAC is read. This will also replace the data segment with the global variables and string literals.
- **TAC Format**: `start proc func`
- **Assembly Translation**:
```asm
main PROC
     MOV AX, @data
     MOV DS, AX
     CALL func
     MOV AH, 04ch
     int 21h
main ENDP
     END main
```

---

### Binary Operations

#### Addition (`+`)
The register AX is used for the addition of two integers.
- **TAC Format**: `t1 = t2 + t3`
- **Assembly Translation**:  
```asm
MOV AX, (t2)    ; Load the value of t2 into AX register
ADD AX, (t3)    ; Add the value of t3 to AX
MOV (t1), AX    ; Store the result from AX into t1
```

#### Subtraction (`-`)
- **TAC Format**: `t1 = t2 - t3`
- **Assembly Translation**:  
```asm
MOV AX, (t2)    ; Load the value of t2 into the AX register
SUB AX, (t3)    ; Subtract the value of t3 from value in AX and store in AX
MOV (t1), AX    ; Store the result from AX into t1
```

#### Multiplication (`*`)
- **TAC Format**: `t1 = t2 * t3`
- **Assembly Translation**:  
```asm
MOV AX, (t2)    ; Load the value of t2 into the AX register
MOV BX, (t3)    ; Load the value of t3 into the BX register
IMUL BX         ; Multiply values in AX and BX registers together and store into AX
MOV (t1), AX    ; Store the result from AX into t1
```

#### Division (`/`)
- **TAC Format**: `t1 = t2 / t3`
- **Assembly Translation**:  
```asm
MOV AX, (t2)    ; Load the value of t2 into the AX register
MOV BX, (t3)    ; Load the value of t3 into the BX register
IDIV BX         ; Divide AX by BX and store result into AX
MOV (t1), AX    ; Store the result from AX into t1
```

#### Remainder (`rem`)
> **Note**: `rem` is not implemented for Intel 8086 Assembly
- **TAC Format**: `t1 = t2 rem t3`
- **Assembly Translation**:  
```asm
MOV AX, (t2)    ; Load the value of t2 into the AX register
MOV BX, (t3)    ; Load the value of t3 into the BX register
IDIV BX         ; Divide AX by BX and store remainder into DX
MOV (t1), DX    ; Store the result from AX into t1
```

#### Remainder (`mod`)
> **Note**: `mod` is not implemented for Intel 8086 Assembly
- **TAC Format**: `t1 = t2 mod t3`
- **Assembly Translation**:  
**UNSURE**
```asm
MOV AX, (t2)    ; Load the value of t2 into the AX register
MOV BX, (t3)    ; Load the value of t3 into the BX register
IDIV BX         ; Divide AX by BX and store remainder into DX
MOV (t1), DX    ; Store the result from AX into t1
```

---

### Unary Operations

#### Copy
- **TAC Format**: `t1 = t2`
- **Assembly Translation**:
```asm
MOV AX, (t2)  ; Load t2 into AX
MOV (t1), AX  ; Store AX into t1
```

#### Positive (`+`)
> **Note**: `+` is identical to copy
- **TAC Format**: `t1 = +t2`
- **Assembly Translation**:  
```asm
MOV AX, (t2)  ; Load t2 into AX
MOV (t1), AX  ; Store AX into t1
```

#### Negative (`-`)
- **TAC Format**: `t1 = -t2`
- **Assembly Translation**:  
```asm
MOV AX, (t2)  ; Load t2 into AX
NEG AX        ; Negate value in AX
MOV (t1), AX  ; Store result in t1
```

---

### Procedures

#### Stack Frame
The process of calling a procedure requires the following steps:
1. Parameters are pushed to the stack (These are pushed from left to right order)
2. Procedure is called (Return Address pushed to stack)
3. Base Pointer is pushed (Base Pointer is now the current stack pointer)
4. Space for local variables are created on stack

Stack variables will be accessed by their offset from the base pointer. Parameters will need a shift of **+2** and local variables will need a shift of **-2** from their TAC values. Below is a table demonstrating the stack structure with pointers, memory addresses, and values during an example procedure call:

```ada
procedure fun(a,b : INTEGER) is
    c : INTEGER;
    d : REAL;
begin
end
```

| Pointer       | Memory Address | Value                  |
|---------------|----------------|------------------------|
|               | `0x100E`       | Parameter 1 (int)      |
|               | `0x100C`       | Parameter 2 (int)      |
|               | `0x100A`       | Return Address         |
| **BP**        | `0x1008`       | Old Base Pointer       | 
|               | `0x1006`       | Local Var 1 (int)      |
|               | `0x1004`       | Local Var 1 (float)    |
| **SP**        | `0x1000`       |                        |

#### Push
- **TAC Format**: `push a`
- **Assembly Translation**:
```asm
PUSH a
```

#### Procedure Header
- **TAC Format**: `proc func`
- **Assembly Translation**:
```asm
func PROC                      ; Start func
     PUSH BP                   ; Save BP
     MOV BP, SP                ; Set new BP
     SUB SP, ($SIZE_OF_LOCALS) ; Size of Local variables (Will be rewritten by endp func)
```

#### Procedure Footer
- **TAC Format**: `endp func`
- **Assembly Translation**:
```asm
     ADD SP, ($SIZE_OF_LOCALS)  ; Size of Local variables
     POP BP                     ; Restore previous BP
     RET ($SIZE_OF_PARAMS)      ; Return and pop params
func ENDP                       ; End func
```

#### Call
- **TAC Format**: `call func`
- **Assembly Translation**:  
```asm
CALL func       ; Calls func
```

---

### IO

#### Write Integer (`wri`)
- **TAC Format**: `wri t1`
- **Assembly Translation**:
```asm
MOV AX, (t1)  ; Move value in t1 to AX
CALL writeint
```

#### Write String (`wrs`)
- **TAC Format**: `wrs t1`
- **Assembly Translation**:
```asm
MOV DX, &t1  ; Move address of t1 to DX
CALL writestr
```

#### Write String (`wrln`)
- **TAC Format**: `wrln`
- **Assembly Translation**:
```asm
CALL writeln
```

#### Read Integer (`rdi`)
- **TAC Format**: `rdi t1`
- **Assembly Translation**:
```asm
CALL writeint
MOV BX, (t1)  ; Move read value to t1
```

---

## Examples

### test85.ada
**ADA Code**
```ada
procedure two is
  procedure x is
  a,b,cc,d:integer;
  begin
    put("Enter a number ");
    get(a);
    b:=10;
    d:=20;
    cc:=d+a*b;
    put("The answer is ");
    putln(cc);
  end x;
begin
   x();
end two;
```

**Generated TAC Code**
```
proc x
wrs _S0
rdi _bp-2
_bp-4 = 10 //or _bp-10 = 10, _bp-4=_bp-10
_bp-8 = 20
_bp-10 = _bp-2 * _bp-4
_bp-12 = _bp-8 + _bp-10
_bp-6 = _bp-12
wrs _S1
wri _bp-6
wrln
endp x

proc two
call x
endp two

start proc two
```

**Translated Intel 8086 Assembly**
```asm
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
```

---

## References

- [Intel 8086 Instruction Set](https://www.eng.auburn.edu/~sylee/ee2220/8086_instruction_set.html)

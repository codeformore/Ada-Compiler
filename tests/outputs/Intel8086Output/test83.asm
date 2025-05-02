        .model small             
        .586                     
        .stack 100h              
        .data                    
a       DW   ?
b       DW   ?
cc      DW   ?
d       DW   5
_S0     DB   "Enter a: ","$"
_t0     DW   ?
_t1     DW   ?
_S1     DB   "c: ","$"
        .code                    
        include io.asm           

one     PROC                     
        PUSH BP                  
        MOV BP, SP               
        SUB SP, 10 

        MOV DX, offset _S0       
        CALL writestr            

        CALL readint             
        MOV a, BX                

        MOV AX, 10               
        MOV b, AX                

        MOV AX, a                
        ADD AX, b                
        MOV _t0, AX              

        MOV AX, _t0              
        MOV cc, AX               

        MOV AX, cc               
        ADD AX, 5                
        MOV _t1, AX              

        MOV AX, _t1              
        MOV cc, AX               

        MOV DX, offset _S1       
        CALL writestr            

        MOV AX, cc               
        CALL writeint            

        CALL writeln             

        ADD SP, 10               
        POP BP                   
        RET 0                    
one     ENDP                     

main    PROC                     
        MOV AX, @data            
        MOV DS, AX               
        CALL one                 
        MOV AH, 04ch             
        int 21h                  
main    ENDP                     
        END main                 

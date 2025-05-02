        .model small             
        .586                     
        .stack 100h              
        .data                    
a       DW   ?
b       DW   ?
cc      DW   ?
d       DW   ?
_S0     DB   "fun c: ","$"
_t2     DW   ?
_t3     DW   ?
_S1     DB   "five c:","$"
        .code                    
        include io.asm           

fun     PROC                     
        PUSH BP                  
        MOV BP, SP               
        SUB SP, 6  

        MOV AX, [BP+6]           
        MOV BX, [BP+4]           
        IMUL BX                  
        MOV [BP-4], AX           

        MOV AX, [BP-4]           
        ADD AX, [BP+6]           
        MOV [BP-6], AX           

        MOV AX, [BP-6]           
        MOV cc, AX               

        MOV DX, offset _S0       
        CALL writestr            

        MOV AX, cc               
        CALL writeint            

        CALL writeln             

        ADD SP, 6                
        POP BP                   
        RET 4                    
fun     ENDP                     

five    PROC                     
        PUSH BP                  
        MOV BP, SP               
        SUB SP, 12 

        MOV AX, 5                
        MOV a, AX                

        MOV AX, 10               
        MOV b, AX                

        MOV AX, 20               
        MOV d, AX                

        MOV AX, a                
        MOV BX, b                
        IMUL BX                  
        MOV _t2, AX              

        MOV AX, d                
        ADD AX, _t2              
        MOV _t3, AX              

        MOV AX, _t3              
        MOV cc, AX               

        MOV DX, offset _S1       
        CALL writestr            

        MOV AX, cc               
        CALL writeint            

        CALL writeln             

        PUSH a                   
        PUSH b                   
        CALL fun                 

        ADD SP, 12               
        POP BP                   
        RET 0                    
five    ENDP                     

main    PROC                     
        MOV AX, @data            
        MOV DS, AX               
        CALL five                
        MOV AH, 04ch             
        int 21h                  
main    ENDP                     
        END main                 

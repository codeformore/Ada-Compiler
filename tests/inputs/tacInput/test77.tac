PROC    fun             
        _BP-2             =  _BP+2             *  _BP+4           
        _BP+0             =  _BP-2           
        _BP-4             =  3                 *  _BP+0           
        _BP+4             =  _BP-4           
ENDP    fun             
PROC    five            
        a                 =  5               
        b                 =  10              
        d                 =  20              
        _t2               =  a                 *  b               
        _t3               =  d                 +  _t2             
        c                 =  _t3             
        push    a               
        push    &b              
        call    fun             
ENDP    five            
START   PROC    five            

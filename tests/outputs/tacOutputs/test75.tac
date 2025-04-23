PROC	fun
		_bp-2	=	_bp+2	*	_bp+4	or	_bp-4	=	_bp+6	*	_bp+4
		_bp-0	=	_bp-2 				or	_bp-2	=	_bp-4
ENDP	fun 
PROC	five
		a		=	5
		b 		=	10
		d		=	20
		_t1		=	a	*	b 
		_t2		=	d	+	_t1
		c		=	_t2	

		push	a
		push	b
		call	fun
ENDP	five
START 	PROC	five
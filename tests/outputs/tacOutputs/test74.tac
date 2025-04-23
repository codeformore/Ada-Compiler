PROC 	one 
		a		=		5
		b		=		10
		_bp-2	=		20				or	_bp-4	=		20
		_bp-4	=		a 	*	b 		or	_bp-6	=		a 	*	b
		_bp-6	=		_bp-2	+ _bp-4 or  _bp-8	=		_bp-4	+ _bp-6
		_bp+0	=		_bp-6			or 	c	=	_bp-8
ENDP	one
PROC 	four
		call one
ENDP	four
START	PROC	four
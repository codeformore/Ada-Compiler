procedure five is
 a,b,cc,d:integer;
 procedure fun(a:integer; inout b:integer) is
  cc:integer;
 begin
  putln(a);
  putln(b);
  cc:=a*b;
  b:=3*cc;
  putln("fun c: ", cc);
 end fun;
begin
  a:=5;
  b:= 10;
  d:= 20;
  cc:= d + a * b;
  putln("five b: ", b);
  fun(a,b);
  putln("five b after fun: ", b);
end five;
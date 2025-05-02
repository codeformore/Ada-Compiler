procedure five is
 a,b,cc,d:integer;
 procedure fun(a:integer; b:integer) is
  c:integer;
 begin
  cc:=a*b + a;
  putln("fun c: ", cc);
 end fun;
begin
  a:=5;
  b:= 10;
  d:= 20;
  cc:= d + a * b;
  putln("five c:", cc);
  fun(a,b);
end five;
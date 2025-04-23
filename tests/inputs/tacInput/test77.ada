procedure five is
 a,b,c,d:integer;
 procedure fun(a:integer; inout b:integer) is
  c:integer;
 begin
  c:=a*b;
  b:=3*c;
 end fun;
begin
  a:=5;
  b:= 10;
  d:= 20;
  c:= d + a * b;

  fun(a,b);
end five;
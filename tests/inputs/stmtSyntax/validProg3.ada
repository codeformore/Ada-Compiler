PROCEDURE main is
 a,b,c:INTEGER; 
  PROCEDURE test1(d:INTEGER; e:INTEGER; OUT f:INTEGER) is
  BEGIN
      f := e * d + 30;
   END test1;
BEGIN
   a := 10;
   b := 20;
END main;

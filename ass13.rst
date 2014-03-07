


0. 
   :stack: NILL 0
   :Volgende: num(2)
   :Rest: +x*3
   :Actie: s5



1. 
   :stack: NILL 0 num(2) 5
   :Volgende: ADD
   :Rest: x*3
   :Actie: r9


2. 
   :stack: NILL 0 Factor 3
   :Volgende: ADD
   :Rest: x*3
   :Actie: r7

3. 
   :stack: NILL 0 Term 2
   :Volgende: ADD
   :Rest: x*3
   :Actie: r4


4. 
   :stack: NILL 0 Expr 1
   :Volgende: ADD
   :Rest: x*3
   :Actie: s7



5. 
   :stack: NILL 0 Expr 1 ADD 7
   :Volgende: ID(x)
   :Rest: \*3
   :Actie: s6

6. 
   :stack: NILL 0 Expr 1 ADD 7 ID(x) 6
   :Volgende: MUL
   :Rest: 3
   :Actie: r10


7. 
   :stack: NILL 0 Expr 1 ADD 7 Factor 3
   :Volgende: MUL
   :Rest: 3
   :Actie: r7

8. 
   :stack: NILL 0 Expr 1 ADD 7 Term 18
   :Volgende: MUL
   :Rest: 3
   :Actie: s9

9. 
   :stack: NILL 0 Expr 1 ADD 7 Term 18 mul 9
   :Volgende: 3
   :Rest: 
   :Actie: s5

10. 
   :stack: NILL 0 Expr 1 ADD 7 Term 18 mul 9 num(6) 5 
   :Volgende: 
   :Rest: 
   :Actie: r9

11. 
   :stack: NILL 0 Expr 1 ADD 7 Term 18 mul 9 Factor 3 
   :Volgende: 
   :Rest: 
   :Actie: r5


12. 
   :stack: NILL 0 Expr 1 ADD 7 Term 18  
   :Volgende: 
   :Rest: 
   :Actie: r3

13. 
   :stack: NILL 0 Expr 1 
   :Volgende: 
   :Rest: 
   :Actie: Accept

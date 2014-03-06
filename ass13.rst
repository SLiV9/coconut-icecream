


0. 
   :stack: leeg
   :Volgende: CINT(2)
   :Rest: +x*3
   :Actie: Shift


1. 
   :stack: CINT(2)
   :Volgende: \+
   :Rest: x*3
   :Actie: Reduce (CINT(2) -> Getal)

2. 
   :stack: Getal
   :Volgende: \+
   :Rest: x*3
   :Actie: Reduce (Getal -> Factor)

3. 
   :stack: Factor
   :Volgende: \+
   :Rest: x*3
   :Actie: Reduce (Factor -> Term)

4. 
   :stack: Term
   :Volgende: \+
   :Rest: x*3
   :Actie: Shift

5. 
   :stack: Term \+
   :Volgende: ID(x)
   :Rest: *3
   :Actie: Shift

6. 
   :stack: Term \+ ID(X)
   :Volgende: \*
   :Rest: 3
   :Actie: Reduce (ID(x) -> Getal)


7. 
   :stack: Term \+ Getal
   :Volgende: \*
   :Rest: 3
   :Actie: Reduce (Getal -> Factor)

8. 
   :stack: Term \+ Factor
   :Volgende: \*
   :Rest: 3
   :Actie: Shift


9. 
   :stack: Term \+ Factor \*
   :Volgende: CINT(3)
   :Rest: 
   :Actie: Shift

10. 
   :stack: Term \+ Factor \* CINT(3)
   :Volgende: 
   :Rest: 
   :Actie: Reduce (CINT(3)->Getal)

11. 
   :stack: Term \+ Factor \* Getal
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Getal->Factor)

12. 
   :stack: Term \+ Factor \* Factor
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Factor*Factor->Factor)

13. 
   :stack: Term \+ Factor
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Factor->Term)

 
14. 
   :stack: Term \+ Term
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Factor->Term)

15. 
   :stack: Term \+ Term
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Term+Term->Term)

16. 
   :stack: Term
   :Volgende: 
   :Rest: 
   :Actie: Reduce (Term->Expression)

17. 
   :stack: Expression
   :Volgende: 
   :Rest: 
   :Actie: Done
 

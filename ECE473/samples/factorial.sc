(define (if-function p q r) (if p q r))

(define (factorial n)
 (if-function (= n 0)
	      1
	      (* n (factorial (- n 1)))))

(define (factorial n)
 (if (= n 0)
     1
     (* n (factorial (- n 1)))))

(not x)

(and p q)

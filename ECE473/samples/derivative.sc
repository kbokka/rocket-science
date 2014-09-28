;;; (derivative expression) ==> expression
;;; (simple-derivative simple-expression) ==> expression

;;; A value is a number.
;;; A variables is a symbol.

;;; An expression is:
;;;  - a value,
;;;  - a variable,
;;;  - (+ e1 ... en), where e1, ..., en are expressions,
;;;  - (- e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (* e1 ... en), where e1, ..., en are expressions,
;;;  - (/ e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (sqrt e), where e is an expression, or
;;;  - (expt e1 e2), where e1 and e2 are expressions.

;;; A simple expression is:
;;;  - a value,
;;;  - a variable,
;;;  - (+ e1 e2), where e1 and e2 are simple expressions,
;;;  - (* e1 e2), where e1 and e2 are simple expressions, or
;;;  - (expt e1 e2), where e1 and e2 are expressions.

(define (simple-derivative e)
 (cond ((number? e) 0)
       ((symbol? e) (if (eq? e 'x) 1 0))
       ((list? e)
	(if (null? e)
	    (panic "Invalid expression")
	    (case (first e)
	     ((+)
	      (if (= (length e) 3)
		  (list '+
			(simple-derivative (second e))
			(simple-derivative (third e)))
		  (panic "not a simple expression")))
	     ((*)
	      (if (= (length e) 3)
		  (list '+
			(list '* (second e) (simple-derivative (third e)))
			(list '* (third e) (simple-derivative (second e))))
		  (panic "not a simple expression")))
	     ((expt)
	      (if (= (length e) 3)
		  (if (number? (third e))
		      (list '*
			    (third e)
			    (list 'expt (second e) (- (third e) 1))
			    (simple-derivative (second e)))
		      (panic "can't (yet) handle this case"))
		  (panic "not a simple expression")))
	     (else (panic "Invalid expression")))))
       (else (panic "Invalid expression"))))

(define (derivative e) (simplify (simple-derivative (simplify e))))

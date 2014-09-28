;;; (calculate expression) ===> value

;;; A value is a number.

;;; An expression is:
;;;  - a value,
;;;  - (+ e1 ... en), where e1, ..., en are expressions,
;;;  - (- e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (* e1 ... en), where e1, ..., en are expressions,
;;;  - (/ e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (sqrt e), where e is an expression, or
;;;  - (expt e1 e2), where e1 and e2 are expressions.

(define (calculate e)
 (cond ((number? e) e)
       ((list? e)
	(if (null? e)
	    (panic "Invalid expression")
	    (case (first e)
	     ((+) (map-reduce + 0 calculate (rest e)))
	     ((-)
	      (case (length (rest e))
	       ((0) (panic "- must take at least one argument"))
	       ((1) (- (calculate (second e))))
	       (else (- (calculate (second e))
			(map-reduce + 0 calculate (rest (rest e)))))))
	     ((*) (map-reduce * 1 calculate (rest e)))
	     ((/)
	      (case (length (rest e))
	       ((0) (panic "/ must take at least one argument"))
	       ((1) (/ (calculate (second e))))
	       (else (/ (calculate (second e))
			(map-reduce * 1 calculate (rest (rest e)))))))
	     ((sqrt)
	      (if (= (length e) 2)
		  (sqrt (calculate (second e)))
		  (panic "sqrt must take exactly one argument")))
	     ((expt)
	      (if (= (length e) 3)
		  (expt (calculate (second e)) (calculate (third e)))
		  (panic "expt must take exactly two arguments")))
	     (else (panic "Invalid expression")))))
       (else (panic "Invalid expression"))))

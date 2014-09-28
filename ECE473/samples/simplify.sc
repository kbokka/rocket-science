;;; (simplify expression) ===> simple-expression

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

(define (and-function p q) (and p q))

(define (calculable? e)
 (cond ((number? e) #t)
       ((symbol? e) #f)
       ((list? e)
	(if (null? e)
	    (panic "Invalid expression")
	    (map-reduce and-function #t calculable? (rest e))))
       (else (panic "Invalid expression"))))

(define (simplify e)
 (cond
  ((calculable? e) (calculate e))
  ((number? e) e)
  ((symbol? e) e)
  ((list? e)
   (if (null? e)
       (panic "Invalid expression")
       (case (first e)
	((+)
	 (case (length (rest e))
	  ((0) 0)
	  ((1) (simplify (second e)))
	  ((2) (let ((e1 (simplify (second e)))
		     (e2 (simplify (third e))))
		(cond ((equal? e1 0) e2)
		      ((equal? e2 0) e1)
		      ((and (list? e2) (eq? (first e2) '+))
		       (simplify (list '+ (list '+ e1 (second e2)) (third e2))))
		      (else (list '+ e1 e2)))))
	  (else (simplify (list '+ (second e) (cons '+ (rest (rest e))))))))
	((-)
	 (case (length (rest e))
	  ((0) (panic "- must take at least one argument"))
	  ((1) (simplify (list '* -1 (second e))))
	  (else (simplify
		 (list '+ (second e) (list '- (cons '+ (rest (rest e)))))))))
	((*)
	 (case (length (rest e))
	  ((0) 1)
	  ((1) (simplify (second e)))
	  ((2) (let ((e1 (simplify (second e)))
		     (e2 (simplify (third e))))
		(cond ((equal? e1 0) 0)
		      ((equal? e2 0) 0)
		      ((equal? e1 1) e2)
		      ((equal? e2 1) e1)
		      ((and (list? e2) (eq? (first e2) '*))
		       (simplify (list '* (list '* e1 (second e2)) (third e2))))
		      (else (list '* e1 e2)))))
	  (else (simplify (list '* (second e) (cons '* (rest (rest e))))))))
	((/)
	 (case (length (rest e))
	  ((0) (panic "/ must take at least one argument"))
	  ((1) (simplify (list 'expt (second e) -1)))
	  (else (simplify
		 (list '* (second e) (list '/ (cons '* (rest (rest e)))))))))
	((sqrt)
	 (if (= (length e) 2)
	     (simplify (list 'expt (second e) 0.5))
	     (panic "sqrt must take exactly one argument")))
	((expt)
	 (if (= (length e) 3)
	     (let ((e1 (simplify (second e)))
		   (e2 (simplify (third e))))
	      (cond ((equal? e1 1) 1)
		    ((equal? e2 1) e1)
		    (else (list 'expt e1 e2))))
	     (panic "expt must take exactly two arguments")))
	(else (panic "Invalid expression")))))
  (else (panic "Invalid expression"))))

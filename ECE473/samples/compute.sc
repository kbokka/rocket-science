;;; (compute expression bindings definitions) ===> value

;;; A value is a number.
;;; A variable is a symbol.
;;; A function is a symbol.

;;; An expression is:
;;;  - a value,
;;;  - a variable,
;;;  - (+ e1 ... en), where e1, ..., en are expressions,
;;;  - (- e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (* e1 ... en), where e1, ..., en are expressions,
;;;  - (/ e1 e2 ... en), where e1, e2, ..., en are expressions,
;;;  - (sqrt e), where e is an expression,
;;;  - (expt e1 e2), where e1 and e2 are expressions,
;;;  - (derivative e), where e is an expression,
;;;  - (if e1 e2 e3), where e1, e2, and e3 are expressions, or
;;;  - (f e1 ... en), where f is a function and e1, ..., en are expressions.

;;; A definition is:
;;;  - (define (f x1 ... xn) e) where f is a function, x1, ..., xn are
;;;    variables, and e is an expression.

;;; A binding is (x v) where x is a variable and v is a value.

(define (lookup x bindings)
 (cond ((null? bindings) (panic "Undefined variable"))
       ((eq? x (first (first bindings))) (second (first bindings)))
       (else (lookup x (rest bindings)))))

(define (lookup-definition f definitions)
 (cond ((null? definitions) (panic "Undefined function"))
       ((eq? f (first (second (first definitions)))) (first definitions))
       (else (lookup f (rest definitions)))))

(define (make-bindings variables values)
 (if (null? variables)
     (if (null? values)
	 '()
	 (panic "Too many arguments"))
     (if (null? values)
	 (panic "Too few arguments")
	 (cons (list (first variables) (first values))
	       (make-bindings (rest variables) (rest values))))))

(define (compute e bindings definitions)
 (define (compute-with-stuff e) (compute e bindings definitions))
 (cond ((number? e) e)
       ((symbol? e) (lookup e bindings))
       ((list? e)
	(if (null? e)
	    (panic "Invalid expression")
	    (case (first e)
	     ((+) (map-reduce + 0 compute-with-stuff (rest e)))
	     ((-)
	      (case (length (rest e))
	       ((0) (panic "- must take at least one argument"))
	       ((1) (- (compute-with-stuff (second e))))
	       (else (- (compute-with-stuff (second e))
			(map-reduce + 0 compute-with-stuff (rest (rest e)))))))
	     ((*) (map-reduce * 1 compute-with-stuff (rest e)))
	     ((/)
	      (case (length (rest e))
	       ((0) (panic "/ must take at least one argument"))
	       ((1) (/ (compute-with-stuff (second e))))
	       (else (/ (compute-with-stuff (second e))
			(map-reduce * 1 compute-with-stuff (rest (rest e)))))))
	     ((sqrt)
	      (if (= (length e) 2)
		  (sqrt (compute-with-stuff (second e)))
		  (panic "sqrt must take exactly one argument")))
	     ((expt)
	      (if (= (length e) 3)
		  (expt (compute-with-stuff (second e))
			(compute-with-stuff (third e)))
		  (panic "expt must take exactly two arguments")))
	     ((derivative)
	      (if (= (length e) 2)
		  (compute-with-stuff (derivative (second e)))
		  (panic "derivative must take exactly one argument")))
	     ((yen)
	      (if (= (length e) 4)
		  (if (zero? (compute-with-stuff (second e)))
		      (compute-with-stuff (third e))
		      (compute-with-stuff (fourth e)))
		  (panic "if must take exactly three arguments")))
	     (else (let ((values (map compute-with-stuff (rest e)))
			 (definition (lookup-definition (first e) definitions)))
		    (compute (third definition)
			     (make-bindings (rest (second definition)) values)
			     definitions))))))
       (else (panic "Invalid expression"))))

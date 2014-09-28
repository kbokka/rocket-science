(define (attacks? qi qj delta-rows)
 (or (= qi qj) (= (abs (- qi qj)) delta-rows)))

(define (check-queens new-column old-columns)
 (for-each-indexed
  (lambda (old-column i)
   (when (attacks? new-column old-column (+ i 1))
    (fail)))
  old-columns))

(define (place-n-queens-by-backtracking n)
 (define (loop columns row)
  (if (= (length columns) n)
      columns
      (let ((column (an-integer-between 0 (- n 1))))
       (check-queens column columns)
       (place-queen row column)
       (loop (cons column columns) (+ row 1)))))
 (loop '() 0))

(define (place-n-queens-by-constraints n)
 (define (*columns*) (map-n (lambda (i) i) n))

 (define (apply-constraints domain-vars)
  (cond ((= (length domain-vars) 1) #f)
	(else
	 (for-each-indexed
	  (lambda (new i)
	   (assert-constraint!
	    (lambda (new old)
	     (not (attacks? new old (+ i 1))))
	    (list (first domain-vars) new)))
	  (rest domain-vars))
	 (apply-constraints (rest domain-vars)))))
 
 (let ((n-domain-variables
	(map-n (lambda (i) (create-domain-variable (*columns*))) n)))

  ;; place queen
  (for-each-indexed
   (lambda (domain-variable i)
    (attach-after-demon!
     (lambda ()
      (when (bound? domain-variable)
       (place-queen i (binding domain-variable))))
     domain-variable))
   n-domain-variables)

  ;; constraints
  (apply-constraints n-domain-variables)

  (csp-solution n-domain-variables first)))



(define (assert-unary-constraint-gfc! constraint x)
 (restrict-domain!
  x
  (remove-if-not constraint (domain-variable-domain x))))

(define (assert-binary-constraint-gfc! constraint x y)
 (for-each
  (lambda (v)
   (attach-after-demon!
    (lambda ()
     (when (bound? x)
      (restrict-domain!
       y
       (remove-if-not (lambda (ye) (constraint (binding x) ye))
		      (domain-variable-domain y))))
     (when (bound? y)
      (restrict-domain!
       x
       (remove-if-not (lambda (xe) (constraint (binding y) xe))
		      (domain-variable-domain x)))))
    v))
  (list x y)))

(define (assert-unary-constraint-ac! constraint x)
 (restrict-domain!
  x
  (remove-if-not constraint (domain-variable-domain x))))

(define (assert-binary-constraint-ac! constraint x y)
 (define (constrained x1 x2) ;; constrain x1 by checking with x2
  (remove-if-not
   (lambda (x1e)
    (some (lambda (x2e) (constraint x1e x2e)) (domain-variable-domain x2)))
   (domain-variable-domain x1)))
 
 (attach-after-demon!
  (lambda () (restrict-domain! x (constrained x y))) x)

 (attach-after-demon!
  (lambda () (restrict-domain! x (constrained x y))) y))
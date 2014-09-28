(define (assert-unary-constraint-gfc! constraint x)
 (restrict-domain!
  x (remove-if-not (lambda (xe) (constraint xe))
                   (domain-variable-domain x))))

(define (assert-binary-constraint-gfc! constraint x y)
 (attach-after-demon!
  (lambda ()
   (when (bound? x)
    (restrict-domain!
     y (remove-if-not (lambda (ye) (constraint (binding x) ye))
                      (domain-variable-domain y)))))
  x)
 (attach-after-demon!
  (lambda ()
   (when (bound? y)
    (restrict-domain!
     x (remove-if-not (lambda (xe) (constraint xe (binding y)))
                      (domain-variable-domain x)))))
  y))

(define (assert-unary-constraint-ac! constraint x)
 (restrict-domain!
  x (remove-if-not (lambda (xe) (constraint xe))
                   (domain-variable-domain x))))

(define (assert-binary-constraint-ac! constraint x y)
 (attach-after-demon!
  (lambda ()
   (restrict-domain!
    y (remove-if-not (lambda (ye)
                      (some (lambda (xe) (constraint xe ye))
                            (domain-variable-domain x)))
                     (domain-variable-domain y))))
  x)
 (attach-after-demon!
  (lambda ()
   (restrict-domain!
    x (remove-if-not (lambda (xe)
                      (some (lambda (ye) (constraint xe ye))
                            (domain-variable-domain y)))
                     (domain-variable-domain x))))
  y))

(define (num-intersects todo-entry check-entries)
 (cond ((null? check-entries) 0)
       ((entries-intersect? todo-entry (first check-entries))
	(+ 1 (num-intersects todo-entry (rest check-entries))))
       (else (num-intersects todo-entry (rest check-entries)))))

(define (intersects todo-entry check-entries)
 (cond ((null? check-entries) '())
       ((entries-intersect? todo-entry (first check-entries))
	(cons (first check-entries)
	      (intersects todo-entry (rest check-entries))))
       (else (intersects todo-entry (rest check-entries)))))

(define (sort-entries entries)
 (let ((sorted-entries 
	(sort entries > (lambda (e) (num-intersects e entries)))))
  (define (order ee)
   (if (null? ee) '()
       (let ((entry-n-followers
	      (cons (first ee) (intersects (first ee) (rest ee)))))
	(append entry-n-followers
		(order
		 (remove-if (lambda (x)
			     (not (not (member x entry-n-followers))))
			    ee))))))
  (order sorted-entries)))
 

(define (a-word all-words)
 (when (null? all-words) (fail))
 (either (first all-words) (a-word (rest all-words))))

(define (check-words new-entry-word old-entry-words)
 (for-each
  (lambda (old-entry-word)
   (when (not (consistent-entries? (first new-entry-word)
				   (first old-entry-word)
				   (second new-entry-word)
				   (second old-entry-word)))
    (fail)))
  old-entry-words))

(define (filter-words words length)
 (remove-if-not (lambda (word) (= (string-length word) length))
		words))

(define (solve-crossword-puzzle-by-backtracking entries all-words)
 (define (loop entry-words entries-left)
  (if (null? entries-left)
      entry-words
      (let ((word (a-word
		   (filter-words all-words
				 (entry-length (first entries-left))))))
       (check-words (list (first entries-left) word) entry-words)
       (fill-entry (first entries-left) word)
       (loop (cons (list (first entries-left) word) entry-words) 
	     (rest entries-left)))))
 (loop '() (sort-entries entries)))

(define (solve-crossword-puzzle-by-constraints entries words)
 (define (apply-constraints mapping)
  (for-each
   (lambda (m1)
    (for-each
     (lambda (m2)
      (when (entries-intersect? (first m1) (first m2))
       (assert-constraint!
	(lambda (w1 w2)
	 (consistent-entries? (first m1) (first m2) w1 w2))
	(list (second m1) (second m2)))))
     mapping))
   mapping))
 
 (let* ((domain-variables
	 (map
	  (lambda (e)
	   (create-domain-variable (filter-words words (entry-length e))))
	  entries))
	(mappings
	 (map list entries domain-variables)))

  ;; fill entry
  (for-each
   (lambda (mapping)
    (attach-after-demon!
     (lambda ()
      (when (bound? (second mapping))
       (fill-entry (first mapping) (binding (second mapping)))))
     (second mapping)))
   mappings)

  ;; constraint
  (apply-constraints mappings)
  
  ;; solve
  (csp-solution domain-variables first)))
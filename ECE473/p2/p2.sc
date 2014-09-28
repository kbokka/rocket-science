(define (true-false vars)
 (cond ((null? vars) '())
       ((not (list? vars)) (true-false (list vars)))
       (else (cons 
                (list (list (first vars) #t)
                    (list (first vars) #f))
                     (true-false (rest vars))))))
                
(define (cross-product l)
  (if (null? l) (list '())
      (append-map (lambda (x)
		   (map (lambda (y)
			 (cons x y))
			(cross-product (rest l))))
       (first l))))
       
(define (append-map f l)
  (if (null? l) l
      (append (f (first l)) (append-map f (rest l)))))

(define (instantiate free-vars)
 (cross-product (true-false (setify (flatten free-vars)))))

(define (member? x list)
 (cond ((null? list) #f)
       ((eq? x (first list)) #t)
       (else (member? x (rest list)))))

(define (setify x)
 (define (loop x c)
  (cond ((null? x) c)
        ((member? (first x) c) (loop (rest x) c))
        (else (loop (rest x) (cons (first x) c)))))
 (loop x '()))

(define (flatten l)
 (cond ((null? l) '())
       ((not (list? l)) (list l))
       (else (append (flatten (first l)) (flatten (rest l))))))

(define (map-reduce g identity f l)
 (if (null? l)
     identity
     (map-reduce g (g (f (first l)) identity) f (rest l))))

(define (lookup x bindings)
 (cond ((null? bindings) (panic "Variable is not instantiated"))
       ((eq? x (first (first bindings))) (second (first bindings)))
       (else (lookup x (rest bindings)))))

(define (find-free-vars phi)
  (if (not (list? phi))
      (cond ((null? phi) '())
	    ((symbol? phi) phi)
	    (else '()))
      (cond ((eq? (first phi) 'not)
	      (if (not (null? (rest phi)))
		(if (list? (rest phi)) (find-free-vars (first (rest phi)))
		    (find-free-vars (rest phi)))))
	    ((eq? (first phi) 'and)
	     (map find-free-vars (rest phi)))
	    ((eq? (first phi) 'or)
	     (map find-free-vars (rest phi))) 
	    ((symbol? (first phi)) phi)
	    (else '()))))


(define (and-function x y) (and x y))
(define (or-function x y) (or x y))

(define (valuate phi I)
 (if (not (list? phi))
     (cond ((or (eq? phi #t) (eq? phi #f)) phi)
	   ((symbol? phi) (lookup phi I)))
     (cond ((or (eq? (first phi) #t) (eq? (first phi) #f))
	    (if (not (null? (rest phi)))
		(if (list? phi)(first phi))))
	   ((eq? (first phi) 'not)
	    (if (not (null? (rest phi)))
		(if (list? (rest phi)) (not (valuate (first (rest phi)) I))
		    (not (valuate (rest phi) I)))))
	   ((eq? (first phi) 'and)
	    (if (not (null? (rest phi)))
		(map-reduce and-function #t
			    (lambda (x) (valuate x I)) (rest phi))))
	   ((eq? (first phi) 'or)
	    (if (not (null? (rest phi)))
		(map-reduce or-function #f
			    (lambda (x) (valuate x I)) (rest phi))))
	   ((symbol? (first phi)) (lookup (first phi) I)))))

(define (valuate-all phi I)
 (if (null? I)
     (list '() (valuate phi '()))
     (map (lambda (instant) (list instant (valuate phi instant))) I)))

(define (truth-table phi)
 (let ((I (instantiate (find-free-vars phi))))
  (valuate-all phi I)))

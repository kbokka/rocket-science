(define (initial-board n)
  (define (row-init n)
    (cond ((= n 0) (list))
          (else (cons 0 (row-init (- n 1))))))
  (define (board rows n)
    (cond ((= n 0) (list))
          (else (cons rows (board rows (- n 1))))))
  (board (row-init n) n))

(define (moves b)
 (define (traverse-cols r r-count count)
  (if (null? r) (list)
      (if (= (first r) 0)
	  (cons (list r-count count)
		(traverse-cols (rest r) r-count (+ count 1)))
	  (traverse-cols (rest r) r-count (+ count 1)))))
  
 (define (traverse-rows b count)
  (if (null? b) (list)
      (append (traverse-cols (first b) count 0)
	    (traverse-rows (rest b) (+ count 1)))))
 
 (traverse-rows b 0))

(define (make-move m b)
 (define (replace-row b x y new)
  (if (= x 0)
      (cons (replace-col (first b) y new) (rest b))
      (cons (first b)
	    (replace-row (rest b) (- x 1) y new))))
 
 (define (replace-col r i new)
  (if (= i 0)
      (cons new (rest r))
      (cons (first r)
	    (replace-col (rest r) (- i 1) new))))
 
 (replace-row b (first m) (second m) (player b)))

(define (player b)
 (define (count row sign)
  (cond ((null? row) 0)
	((eq? (first row) sign) (+ (count (rest row) sign) 1))
	(else (+ (count (rest row) sign) 0))))
 (let ((xcount (map-reduce + 0 (lambda (r) (count r 1)) b))
       (ocount (map-reduce + 0 (lambda (r) (count r -1)) b)))
  (if (> xcount ocount) -1 1)))

(define (win b) 
 (define (check-row r)
  (cond ((= (length r) 1) (first r))
	((= (first r) 0) 0)
	((= (first r) (second r)) (check-row (rest r)))
	(else 0)))
 
 (define (check-rows b)
  (cond ((null? b) 0)
	((= (check-row (first b)) 1) 1)
	((= (check-row (first b)) -1) -1)
	(else (check-rows (rest b)))))
 
 (define (check-cols-per-rows r1 r2)
  (cond ((null? r1) (list))
	((null? r2) r1)
	((= (first r1) 0)
	 (cons 0 (check-cols-per-rows (rest r1) (rest r2))))
	((= (first r1) (first r2))
	 (cons (first r1) (check-cols-per-rows (rest r1) (rest r2))))
	(else (cons 0 (check-cols-per-rows (rest r1) (rest r2))))))

 (define (check-cols b)
  (define (find p? list)
   (cond ((null? list) 0)
	 ((p? (first list)) (first list))
	 (else (find p? (rest list)))))
  
  (find (lambda (x) (or (= x 1) (= x -1)))
	      (reduce check-cols-per-rows b '())))

 ;; check right diagonal
 (define (check-diag b)
  (define (remove-col b)
   (cond ((null? b) (list))
	 (else (cons (rest (first b)) (remove-col (rest b))))))
  
  (cond ((= (length b) 1) (first (first b)))
	((= (first (first b)) 0) 0)
	((= (first (first b)) (second (second b)))
	 (check-diag (rest (remove-col b))))
	(else 0)))

 ;; Lazy evaluation of checks
 (let ((row-winner (check-rows b)))
  (if (not (= row-winner 0)) row-winner
      (let ((col-winner (check-cols b)))
       (if (not (= col-winner 0)) col-winner
	   (let ((di1-winner (check-diag b)))
	    (if (not (= di1-winner 0)) di1-winner
		(let ((di2-winner (check-diag (reverse b))))
		 (if (not (= di2-winner 0)) di2-winner 0)))))))))

(define (remove-if-not p? l)
 (cond ((null? l) '())
       ((p? (first l)) (cons (first l) (remove-if-not p? (rest l))))
       (else (remove-if-not p? (rest l)))))

(define (win-* b ll)
 (define (maximize f l limit)
  (define (loop best-so-far l)
   (cond ((>= best-so-far limit) 1)
	 ((null? l) best-so-far)
	 (else (loop (max (f (first l) best-so-far) best-so-far) (rest l)))))
  (loop -1 l))

 (let ((win-0 (win b))
       (m (moves b))
       (player-b (player b)))
  (if (or (not (= win-0 0)) (null? m))
      win-0
      (* player-b
	 (maximize (lambda (m lp) (* player-b
				  (win-* (make-move m b) (* player-b lp))))
		   m
		   (* player-b ll))))))

(define (win~ b)
 (define (approx-win b)
  (define (check-row r)
   (cond ((and (= (length r) 1) (not (= (first r) 0))) (first r))
	 ((and (= (length r) 1) (= (first r) 0)) 0)
	 ((= (first r) 0) (check-row (rest r)))
	 ((or (= (first r) (second r)) (= (second r) 0)) 
	  (check-row (cons (first r) (rest (rest r)))))
	 (else 0)))
  (define (check-cols-per-rows r1 r2)
   (cond ((null? r1) (list))
	 ((null? r2) r1)
	 ((= (first r1) 0)
	  (cons (first r2) (check-cols-per-rows (rest r1) (rest r2))))
	 ((= (first r2) 0)
	  (cons (first r1) (check-cols-per-rows (rest r1) (rest r2))))
	 ((= (first r1) (first r2))
	  (cons (first r1) (check-cols-per-rows (rest r1) (rest r2))))
	 (else (cons 0 (check-cols-per-rows (rest r1) (rest r2))))))

  (define (check-diag b)
   (define (remove-col b)
    (cond ((null? b) (list))
	  (else (cons (rest (first b)) (remove-col (rest b))))))
   
   (cond ((= (length b) 1) (first (first b)))
	 ((= (first (first b)) 0)
	  (check-diag (rest (remove-col b))))
	 ((= (second (second b)) 0)
	  (check-diag (cons (list (first (first b))) 
			    (remove-col (rest (rest b))))))
	 ((= (first (first b)) (second (second b)))
	  (check-diag (rest (remove-col b))))
	 (else 0)))
  
  (+  (reduce + (map check-row b) 0)
      (reduce + (reduce check-cols-per-rows b '()) 0)
      (check-diag b)
      (check-diag (reverse b))))
   
 (let ((win-0 (win b))
       (m (moves b))
       (approximate-win (approx-win b))
       (winnables (* 2 (length b))))
  (cond ((= win-0 1) 1)
	((and (= win-0 0) (null? m) (> approximate-win 0))
	 (/ approximate-win winnables))
	((and (= win-0 0) (null? m) (< approximate-win 0))
	 (/ approximate-win winnables))
	((= win-0 -1) -1)
	((or (= win-0 0) (= approximate-win 0)) 0))))

(define (win~k k b ll)
 (define (maximize f l limit)
  (define (loop best-so-far l)
   (cond ((>= best-so-far limit) 1)
	 ((null? l) best-so-far)
	 (else (loop (max (f (first l) best-so-far) best-so-far) (rest l)))))
  (loop -1 l))
 
 (let ((win-0 (win b))
       (m (moves b))
       (player-b (player b)))
  (cond ((or (not (= win-0 0)) (null? m)) win-0)
	((= k 0) (win~ b))
	(else (* player-b
		 (maximize
		  (lambda (m lp)
		   (* player-b (win~k (- k 1)
				      (make-move m b) (* player-b lp))))
		   m (* player-b ll)))))))
 
(define (optimal-moves~ k b)
 (if (not (= (win b) 0)) (list)
     (let ((player-b (player b)))
      (if (= k Infinity)
	  ;; infinite depth
	  (remove-if-not
	   (lambda (m) (= (win-* (make-move m b) (- 0 player-b))
			  (win-* b player-b))) (moves b))
	  ;; finite depth
	  (remove-if-not
	   (lambda (m) (>= (* player-b
			      (win~k (- k 1) (make-move m b) (- 0 player-b)))
			   (* player-b (win~k k b player-b)))) (moves b))))))
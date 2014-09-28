(define (member? x l)
 (cond ((null? l) #f)
       ((= x (first l)) #t)
       (else (member? x (rest l)))))

(define (remove-all x l)
 (cond ((null? l) (list))
       ((= (first l) x) (remove-all x (rest l)))
       (else (cons (first l) (remove-all x (rest l))))))

(define (remove-dups l) ; remove duplicates
 (if (null? l)
     (list)  
     (cons (first l) (remove-dups (remove-all (first l) (rest l))))))

(define (set-union A B)
 (if (null? A)
     (remove-dups B)
     (remove-dups (cons (first A) (set-union (rest A) B)))))

(define (set-intersection A B)
 (if (null? A)
     (list)
     (if (member? (first A) B)
	 (remove-dups (cons (first A) (set-intersection (rest A) B)))
	 (remove-dups (set-intersection (rest A) B)))))

(define (set-minus A B)
 (if (null? A)
     (list)
     (if (not (member? (first A) B))
	 (remove-dups (cons (first A) (set-minus (rest A) B)))
	 (remove-dups (set-minus (rest A) B)))))
(define (length l)
 (if (null? l)
     0
     (increment (length (rest l)))))

;;; l[i]

(define (list-ref l i)
 (if (zero? i)
     (first l)
     (list-ref (rest l) (decrement i))))

;;; l[i] = x

(define (list-replace-ith l i x)
 (if (zero? i)
     (cons x (rest l))
     (cons (first l) (list-replace-ith (rest l) (decrement i) x))))

(define (list-insert-ith l i x)
 (if (zero? i)
     (cons x l)
     (cons (first l) (list-insert-ith (rest l) (decrement i) x))))

(define (list-remove-ith l i)
 (if (zero? i)
     (rest l)
     (cons (first l) (list-remove-ith (rest l) (decrement i)))))

(define (member? x l)
 (if (null? l)
     #f
     (if (= x (first l))
	 #t
	 (member? x (rest l)))))

(define (member? x l)
 (cond ((null? l) #f)
       ((= x (first l)) #t)
       (else (member? x (rest l)))))

(define (position x l)
 (cond ((null? l) #f)
       ((= (first l) x) 0)
       (else (let ((result (position x (rest l))))
	      (if (eq? result #f)
		  #f
		  (increment result))))))

(define (position x l)
 (cond ((null? l) (panic "Not in list"))
       ((= (first l) x) 0)
       (else (increment (position x (rest l))))))

(define (remove-one x l)
 (cond ((null? l) (list))
       ((= (first l) x) (rest l))
       (else (cons (first l) (remove-one x (rest l))))))

(define (remove-all x l)
 (if (member? x l)
     (remove-all x (remove-one x l))
     l))

(define (remove-all x l)
 (cond ((null? l) (list))
       ((= (first l) x) (remove-all x (rest l)))
       (else (cons (first l) (remove-all x (rest l))))))

(define (replace-one x y l)
 (cond ((null? l) (list))
       ((= (first l) x) (cons y (rest l)))
       (else (cons (first l) (replace-one x y (rest l))))))

(define (replace-all x y l)
 (cond ((null? l) (list))
       ((= (first l) x) (cons y (replace-all x y (rest l))))
       (else (cons (first l) (replace-all x y (rest l))))))

(define (append l1 l2)
 (if (null? l1)
     l2
     (cons (first l1) (append (rest l1) l2))))

;;; (append (list 1 2 3) (list 4 5 6))
;;; (cons (first (list 1 2 3)) (append (rest (list 1 2 3)) (list 4 5 6)))
;;; (cons 1 (append (rest (list 1 2 3)) (list 4 5 6)))
;;; (cons 1 (append (list 2 3) (list 4 5 6)))
;;; (cons 1 (cons (first (list 2 3)) (append (rest (list 2 3)) (list 4 5 6))))
;;; (cons 1 (cons 2 (append (rest (list 2 3)) (list 4 5 6))))
;;; (cons 1 (cons 2 (append (list 3) (list 4 5 6))))
;;; (cons 1 (cons 2 (cons (first (list 3)) (append (rest (list 3)) (list 4 5 6)))))
;;; (cons 1 (cons 2 (cons 3 (append (rest (list 3)) (list 4 5 6)))))
;;; (cons 1 (cons 2 (cons 3 (append (list) (list 4 5 6)))))
;;; (cons 1 (cons 2 (cons 3 (list 4 5 6))))
;;; (cons 1 (cons 2 (list 3 4 5 6)))
;;; (cons 1 (list 2 3 4 5 6))
;;; (list 1 2 3 4 5 6)

(define (reverse l)
 (if (null? l)
     (list)
     (append (reverse (rest l)) (list (first l))) ))

(define (reduce g l identity)
 (if (null? l)
     identity
     (g (first l) (reduce g (rest l) identity))))

(define (reduce g l identity)
 (if (null? l)
     identity
     (reduce g (rest l) (g (first l) identity))))

;;; (increment-list (list 1 2 3 4 5)) ==> (2 3 4 5 6)

(define (map f l)
 (if (null? l)
     (list)
     (cons (f (first l)) (map f (rest l)))))

(define (map-reduce g i f l) (reduce g (map f l) i))

(define (map-reduce g identity f l)
 (if (null? l)
     identity
     (g (f (first l)) (map-reduce g identity f (rest l)))))

(define (map-reduce g identity f l)
 (if (null? l)
     identity
     (map-reduce g (g (f (first l) identity)) f (rest l))))

(define google (map-reduce max minus-infinity page-rank *the-word-wide-web*))

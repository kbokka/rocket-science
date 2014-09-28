;;; A rule is:
;;;  (e1 -~-> e2) where e1 and e2 are patterns

;;; A pattern is:
;;;  a pattern variable,
;;;  an expression constant, or
;;;  (p1 ... pn) where p1 ... pn are patterns

;;; (match p e) ==> either #f or a list of bindings

;;; A binding is:
;;;  (p e) where p is a pattern variable and e is an expression

;;; (rewrite '(+ (* 2 (expt x 3)) 0) '(((+ e 0) -~-> e)))

(define (pattern-variable? p) (member p '(e e1 e2 e3)))

(define (inconsistent-binding? b1 b2)
 (and (eq? (first b1) (first b2)) (not (equal? (second b1) (second b2)))))

(define (inconsistent-bindings? r1 r2)
 (some (lambda (b1) (some (lambda (b2) (inconsistent-binding? b1 b2)) r2)) r1))

(define (merge-results-of-match r1 r2)
 (if (or (eq? r1 #f) (eq? r2 #f) (inconsistent-bindings? r1 r2))
     #f
     (append r1 r2)))

(define (match p e)
 (cond ((pattern-variable? p) (list (list p e)))
       ((list? p)
	(if (and (list? e) (= (length p) (length e)))
	    (reduce merge-results-of-match (map match p e) '())
	    #f))
       (else (if (equal? p e) '() #f))))

;;; (match 'x 'x) ==> ()
;;; (match '+ '*) ==> #f

;;; (match '(e1 e2) '(x y)) ==> ((e1 x) (e2 y))
;;;   (match 'e1 'x) ==> ((e1 x))
;;;   (match 'e2 'y) ==> ((e2 y))

;;; (match '(e1 e1) '(x y)) ==> #f
;;;   (match 'e1 'x) ==> ((e1 x))
;;;   (match 'e1 'y) ==> ((e2 y))

;;; (match '(e1) '(x y)) ==> #f

;;; (match '(e1 e2) '(x)) ==> #f

;;; (match '(+ e2) '(* x)) ==> #f
;;;   (match '+ '*) ==> #f
;;;   (match 'e2 'x) ==> ((e2 x))

;;; (match '(e1 e2) 'x) ==> #f

;;; (and e1 e1) -~-> (and e1)
;;; (and p p)
;;; (match '(and e1 e1) '(and p p)) ==> ((e1 p))

(define (lookup-pattern-variable p bindings)
 (cond ((null? bindings) (panic "unbound pattern variable"))
       ((eq? (first (first bindings)) p) (second (first bindings)))
       (else (lookup-pattern-variable p (rest bindings)))))

(define (instantiate p bindings)
 (define (instantiate-with-bindings p) (instantiate p bindings))
 (cond ((pattern-variable? p) (lookup-pattern-variable p bindings))
       ((list? p) (map instantiate-with-bindings p))
       (else p)))

(define (first-matching-rule e rules)
 (cond ((null? rules) #f)
       ((not (eq? (match (first (first rules)) e) #f)) (first rules))
       (else (first-matching-rule e (rest rules)))))

(define (rewrite e rules)
 (define (rewrite-with-rules e) (rewrite e rules))
 (let ((e (if (list? e) (map rewrite-with-rules e) e)))
  (let ((rule (first-matching-rule e rules)))
   (if (eq? rule #f)
       e
       (rewrite (instantiate (third rule) (match (first rule) e)) rules)))))

;;; (and e1 e1) -~-> (and e1)

;;; (f (and p p)) -~-> (f (and p))

;;; (rewrite '(f (and p p)) *rules*) ==> (f (and p))

(define *rules*
 '(((not #t) -~-> (#f)
    (and e1 e1) -~-> (e1))))

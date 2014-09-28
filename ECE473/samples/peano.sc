(define (increment n) (+ n 1))

(define (decrement n) (- n 1))

(define (plus m n)
 (if (= n 0)
     m
     (increment (plus m (decrement n)))))

(define (plus m n)
 (if (= n 0)
     m
     (plus (increment m) (decrement n))))

(define (minus m n)
 (if (= n 0)
     m
     (minus (decrement m) (decrement n))))

(define (times m n)
 (if (= n 0)
     0
     (plus m (times m (decrement n)))))

(define (divide m n)
 (if (= m 0)
     0
     (increment (divide (minus m n) n))))

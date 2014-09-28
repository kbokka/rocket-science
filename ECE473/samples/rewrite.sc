;;; (rewrite expression rules) ===> expression

;;; A value is
;;; - a number or
;;; - a boolean.

;;; A variable is a symbol.

;;; An expression is
;;; - a value,
;;; - a variable, or
;;; - (e1 e2 ... en), where e1, e2, ... en are expressions.

;;; An expression variables is e, e1, e2, ..., e6.

;;; An expression pattern is
;;; - an expression variable,
;;; - a value,
;;; - a variable, or
;;; - (p1 p2 ... pn), where p1, p2, ... pn are expression patterns.

;;; A rule is (p1 -~-> p2).

;;; A binding is (p e) where p is an expression variable and e is an expression.

;;; (match pattern expression) ===> bindings or #f

(define (expression-variable? p) (member p '(e e1 e2 e3 e4 e5 e6)))

(define (value? p) (or (number? p) (boolean? p)))

(define (variable? p) (symbol? p))

(define (match p e)
 (cond ((expression-variable? p) (list (list p e)))
       ((value? p) (if (equal? p e) '() #f))
       ((variable? p) (if (equal? p e) '() #f))
       ((list? p) ...)
       (else (panic "Invalid pattern"))))

;;; (instantiate pattern bindings) ==> expression

(define (instantiate p bindings)
 ...)

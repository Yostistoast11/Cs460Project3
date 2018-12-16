(define (insert_last ls v)
        (if (list? ls)
        	(if (null? ls)
                	(cons v '())
                	(cons (car ls) (insert_last (cdr ls) v))
		)
        	'first_argument_of_insert_last_must_be_a_list
	)
)

(define (list_reverse ls)
        (if (list? ls)
            	(if (null? ls)
                	'()
                	(insert_last (list_reverse (cdr ls)) (car ls))
		)
            	'list_reverse_requires_a_list_argument)
)

(define (main)
	(display (list_reverse '(indentifier numeric literal cons if display newline
        			 cddr and or not define number? symbol list? zero?
				 null? modulo string? + - / * = > < >= <= 'anything
				 1 1.01 "HelloWorld")))
	(newline)
	(display (cons 'a 'b)) (newline)
)

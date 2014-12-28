#!/usr/bin/gosh

(use gauche.config)
(use srfi-1)

(define (main args)
  (print
    (string-join
      (delete-duplicates
        (append (string-split (gauche-config "-l") " ")
                (cdr args))))))


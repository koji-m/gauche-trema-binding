;;;
;;; gauche_trema
;;;

(define-module net.trema
  (export test-gauche_trema ;; dummy
          *ofpfc-add* *ofpp-flood* *initializer*
          send-flow-mod-add exact-match-from init-handlers
          init-trema start-trema set-switch-ready-handler 
          set-switch-disconnected-handler set-packet-in-handler
          make-action-output send-packet-out match-from
          send-flow-mod debug-print controller launch)
  )
(select-module net.trema)

(define *ofpfc-add* 0)
(define *ofpp-flood* 65531)
(define *initializer* (make-hash-table))

(define (send-flow-mod-add datapath_id ofp_match action . rest)
  (let-optionals* rest ((timeout 60)
                       (priority 65535)
                       (buffer_id -1))
    (send-flow-mod *ofpfc-add* datapath_id ofp_match action timeout priority buffer_id)))

(define (exact-match-from message)
  (match-from message 0))

(define (init-handlers hdl-dfs)
  (if (null? hdl-dfs)
    #t
    (begin
      (let ((init (hash-table-get *initializer* (car hdl-dfs) #f)))
        (if init (apply init '())))
      (init-handlers (cdr hdl-dfs)))))

(define *handlers* '())

(define (controller . handlers)
  (set! *handlers* handlers))

(define-syntax launch
  (syntax-rules ()
    ((launch)
     (define (main args)
       (init-trema args)
       (init-handlers *handlers*)
       (start-trema)))))


;; Loads extension
(dynamic-load "gauche_trema")


(hash-table-put! *initializer* 'switch-ready set-switch-ready-handler)
(hash-table-put! *initializer* 'switch-disconnected set-switch-disconnected-handler)
(hash-table-put! *initializer* 'packet-in set-packet-in-handler)


;;
;; Put your Scheme definitions here
;;




Gauche trema binding.  
###Usage

write message handler in scheme like following.
```scheme
(controller
  ;; define switch-ready handler
  (define (switch-ready datapath_id)  
  ...  )

  ;; define packet-in handler
  (define (packet-in datapath_id message)  
  ...  ))

(launch)
```

learning switch written in scheme.
```scheme
#!/usr/bin/gosh

(use net.trema)

(controller
  (define *fdb* (make-hash-table 'string=?))

  (define (flow-mod datapath_id message action)
    (send-flow-mod-add datapath_id
      (exact-match-from message)
      action))

  (define (flood datapath_id message)
    (send-packet-out datapath_id message
      (make-action-output *ofpp-flood*)))

  (define (packet-in datapath_id message)
    (hash-table-put! *fdb* (ref message 'macsa)
      (ref message 'in-port))
    (let ((port_no (hash-table-get *fdb* (ref message 'macda) #f)))
    (if port_no
      (let ((act (make-action-output port_no)))
        (flow-mod datapath_id message act)
        (send-packet-out datapath_id message act))
        (flood datapath_id message)))))

(launch)
```

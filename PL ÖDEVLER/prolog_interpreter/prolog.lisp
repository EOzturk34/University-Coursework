
(declaim (ftype (function (list list &optional list) list) unify))


(defun variable-p (str)
  (and (stringp str)
       (char<= #\A (char str 0) #\Z)))


(defun unifiable-p (pred1 pred2)
  (and (= (length pred1) (length pred2))
       (string= (first pred1) (first pred2))))


(defun make-substitution (var val)
  (list var val))


(defun apply-substitution (substitution predicate)
  (mapcar #'(lambda (term)
              (if (string= term (first substitution))
                  (second substitution)
                  term))
          predicate))


(defun apply-substitutions (substitutions predicate)
  (reduce #'(lambda (pred subst)
              (apply-substitution subst pred))
          substitutions
          :initial-value predicate))


(defun find-matching-axioms (query axioms)
  (remove-if-not
   #'(lambda (axiom)
       (let ((head (first axiom)))
         (unifiable-p head (first query))))
   axioms))


(defun unify-variable (var-pred term-pred bindings)
  (let ((var (second var-pred)))
    (cond
      
      ((assoc var bindings :test #'string=)
       (unify (apply-substitution (assoc var bindings :test #'string=) var-pred)
              term-pred
              bindings))
      
      (t (cons (make-substitution var (second term-pred)) bindings)))))


(defun unify (pred1 pred2 &optional bindings)
  (cond 
    ((and (null bindings) (equal pred1 pred2))
     '())
    ((variable-p (second pred1))
     (unify-variable pred1 pred2 bindings))
    ((variable-p (second pred2))
     (unify-variable pred2 pred1 bindings))
    ((and (unifiable-p pred1 pred2)
          (every #'equal (rest pred1) (rest pred2)))
     bindings)
    (t nil)))


(defun rename-variables (axiom &optional (counter 0))
  (if (atom axiom)
      axiom
      (mapcar #'(lambda (term)
                  (if (listp term)
                      (mapcar #'(lambda (x)
                                (if (variable-p x)
                                    (progn
                                      (incf counter)
                                      (concatenate 'string x (write-to-string counter)))
                                    x))
                              term)
                      term))
              axiom)))



(defun resolve-query (query axioms bindings &optional (depth 0) (path nil))
  (if (> depth 5)
      nil
      (if (null query)
          (list bindings)
          (let* ((current-goal (first query))
                 (rest-goals (rest query))
                 (matching-axioms (find-matching-axioms (list current-goal) axioms)))
            (if (member current-goal path :test #'equal)
                nil
                (remove nil
                       (mapcan
                        #'(lambda (axiom)
                            (let* ((renamed-axiom (rename-variables axiom))
                                  (rule-head (first renamed-axiom))
                                  (new-bindings (unify rule-head current-goal bindings)))
                              (when new-bindings
                                (if (member "<" renamed-axiom :test #'equal)
                                   
                                    (let* ((body-goals (cddr renamed-axiom))
                                           (substituted-goals 
                                             (mapcar 
                                              #'(lambda (goal)
                                                  (apply-substitutions new-bindings goal))
                                              body-goals)))
                                      (resolve-query 
                                       (append substituted-goals rest-goals)
                                       axioms
                                       new-bindings
                                       (1+ depth)
                                       (cons current-goal path)))
                                    
                                    (resolve-query 
                                     rest-goals 
                                     axioms 
                                     new-bindings 
                                     depth 
                                     path)))))
                        matching-axioms)))))))



(defun prolog_prove (axioms query)
  (let ((results (resolve-query query axioms nil)))
    (if results
        (sort
         (remove-duplicates 
          (remove nil
                  (mapcar #'(lambda (binding)
                             (when binding
                               (let ((x-binding-chain 
                                      (loop for b in binding
                                            when (string= "X" (first b))
                                            collect b)))
                                 (when x-binding-chain
                                   (let ((final-value 
                                           (reduce 
                                            (lambda (current-val b)  
                                              (let ((bound-val (assoc (second b) binding :test #'string=)))
                                                (if bound-val
                                                    (second bound-val)
                                                    (second b))))
                                            x-binding-chain
                                            :initial-value (second (first x-binding-chain)))))
                                     (list "X" final-value))))))
                          results))
          :test #'equal)
         #'(lambda (x y)
             (string< (second x) (second y))))
        nil)))


(defun run-tests ()
  (format t "Running Prolog Tests...~%~%")
  
  
  (let* ((axioms '((("father" "jim" "jill"))))
         (query '(("father" "X" "jill")))
         (result (prolog_prove axioms query)))
    (format t "Test 1: Simple fact query~%")
    (format t "Query: ~A~%" query)
    (format t "Expected: ((X jim))~%")
    (format t "Got: ~A~%" result)
    (format t "Status: ~A~%~%" 
            (if (equal result '(("X" "jim"))) "PASS" "FAIL")))
  
  
  (let* ((axioms '((("father" "jim" "jill"))
                   (("parent" "X" "Y") "<" ("father" "X" "Y"))))
         (query '(("parent" "X" "jill")))
         (result (prolog_prove axioms query)))
    (format t "Test 2: Parent rule~%")
    (format t "Query: ~A~%" query)
    (format t "Expected: ((X jim))~%")
    (format t "Got: ~A~%" result)
    (format t "Status: ~A~%~%" 
            (if (equal result '(("X" "jim"))) "PASS" "FAIL")))

  
  (let* ((axioms '((("father" "jim" "jill"))
                   (("father" "tom" "jill"))
                   (("father" "sam" "jill"))))
         (query '(("father" "X" "jill")))
         (result (prolog_prove axioms query)))
    (format t "Test 3: Multiple matching facts~%")
    (format t "Query: ~A~%" query)
    (format t "Expected: ((X jim) (X tom) (X sam))~%")
    (format t "Got: ~A~%" result)
    (format t "Status: ~A~%~%" 
            (if (and (member '("X" "jim") result :test #'equal)
                    (member '("X" "tom") result :test #'equal)
                    (member '("X" "sam") result :test #'equal))
                "PASS" "FAIL")))


  (let* ((axioms '((("father" "john" "jim"))     
                   (("father" "jim" "jill"))      
                   (("parent" "X" "Y") "<" ("father" "X" "Y"))   
                   (("ancestor" "X" "Y") "<" ("parent" "X" "Y")) 
                   (("ancestor" "X" "Y") "<" ("parent" "X" "Z") ("ancestor" "Z" "Y")))) 
         (query '(("ancestor" "X" "jill")))
         (result (prolog_prove axioms query)))
    (format t "Test 4: Ancestor chain test~%")
    (format t "Query: ~A~%" query)
    (format t "Expected: ((X jim) (X john))~%")
    (format t "Got: ~A~%" result)
    (format t "Status: ~A~%~%" 
            (if (and (member '("X" "jim") result :test #'equal)
                    (member '("X" "john") result :test #'equal))
                "PASS" "FAIL"))))
  

  
    
(defun pdf-example-test ()
  (let ((axioms 
         '(
           (("father" "jim" "jill"))
           (("mother" "mary" "jill"))
           (("father" "samm" "jim"))
           (("parent" "X" "Y") "<" ("father" "X" "Y"))  
           (("parent" "X" "Y") "<" ("mother" "X" "Y"))  
           (("ancestor" "X" "Y") "<" ("parent" "X" "Y"))  
           (("ancestor" "X" "Y") "<" ("parent" "Z" "Y") ("ancestor" "X" "Z"))  
          ))
        (query1 '(("ancestor" "X" "jill")))
        (query2 '(("ancestor" "X" "jill") ("mother" "X" "bob"))))
    
    (format t "~&=== PDF Example Test ===~%")
    (format t "~%Query1: ~A~%" query1)
    (let ((res1 (prolog_prove axioms query1)))
      (format t "Result Query1: ~A~%" res1))

    (format t "~%Query2: ~A~%" query2)
    (let ((res2 (prolog_prove axioms query2)))
      (format t "Result Query2: ~A~%~%" res2))))


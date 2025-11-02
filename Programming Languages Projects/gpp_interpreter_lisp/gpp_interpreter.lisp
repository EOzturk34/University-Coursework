;;;; G++ Interpreter Implementation

;;; Lexer Section
(defun char-alpha-p (char)
  "Karakterin harf olup olmadığını kontrol eder."
  (and char
       (or (char<= #\A char #\Z)
           (char<= #\a char #\z))))

(defun char-digit-p (char)
  "Karakterin rakam olup olmadığını kontrol eder."
  (and char
       (char<= #\0 char #\9)))

(defun valid-identifier-char-p (char)
  "Tanımlayıcı içinde kullanılabilecek karakterleri kontrol eder."
  (or (char-alpha-p char) (char-digit-p char) (char= char #\_)))

(defun parse-operator (char)
  "Operatör karakterlerini tanır ve uygun token'ı döndürür."
  (case char
    (#\+ 'OP_PLUS)
    (#\- 'OP_MINUS)
    (#\* 'OP_MULT)
    (#\/ 'OP_DIV)
    (#\( 'OP_OP)
    (#\) 'OP_CP)
    (#\, 'OP_COMMA)
    (#\' 'OP_QUOTE)))

(defun parse-string (input index)
  "String değerleri parse eder"
  (let ((start-index (1+ index))  ; çift tırnağı atla
        (length (length input)))
    (setf index (1+ index))  ; index'i başlangıç pozisyonuna getir
    (loop while (< index length) do
        (let ((char (char input index)))
          (cond
            ((char= char #\")
             (return-from parse-string 
               (values (list 'VALUESTR (subseq input start-index index)) (1+ index))))
            (t (setf index (1+ index)))))
    ;; Eğer buraya kadar geldiyse string kapatılmamış demektir
    (values (list :error "Unterminated string") index))))

(defun parse-char (input index)
  "Char değerleri parse eder (tek tırnak içindeki tek karakter)"
  (let ((length (length input)))
    (cond
      ;; Yeterli uzunlukta karakter var mı?
      ((< (+ index 2) length)
       (let ((char (char input (1+ index)))         ; karakter
             (next-char (char input (+ index 2))))   ; kapanış tırnağı olmalı
         (if (char= next-char #\')
             ;; Sonraki karakter var mı ve bu karakter bir rakam/harf vs. mi?
             (if (and (< (+ index 3) length)
                     (not (char= (char input (+ index 3)) #\Space))
                     (not (char= (char input (+ index 3)) #\))))
                 (values (list :error "Invalid character literal format") index)
                 (values (list 'VALUECHAR char) (+ index 3)))
             (values (list :error "Invalid character literal - missing closing quote") index))))
      (t 
       (values (list :error "Invalid character literal") index)))))

(defun parse-number (input index)
  "Sayıları parse eder (tam sayı ve kesirli sayı)"
  (let ((start-index index)
        (length (length input))
        (state 'integer)
        numerator
        denominator)
    (loop
      (let ((char (if (< index length)
                     (char input index)
                     nil)))
        (cond
          ;; Rakam
          ((and char (char-digit-p char))
           (setf index (1+ index)))
          ;; Kesir işareti
          ((and char (char= char #\:))
           (if (eq state 'integer)
               (progn
                 (setf numerator (parse-integer (subseq input start-index index)))
                 (setf start-index (1+ index))
                 (setf state 'fraction)
                 (setf index (1+ index)))
               (return)))
          ;; Virgül kontrolü - hata ver
          ((and char (char= char #\,))
           (return-from parse-number (values (list :error "Invalid number format: comma not allowed") index)))
          ;; Nokta kontrolü - hata ver
          ((and char (char= char #\.))
           (return-from parse-number (values (list :error "Invalid number format: decimal point not allowed") index)))
          (t (return)))))
    
    ;; Sayı tipini belirle ve değeri döndür
    (cond
      ;; Kesirli sayı
      ((eq state 'fraction)
       (let ((denom-str (subseq input start-index index)))
         (if (string= denom-str "")
             (values (list :error "Invalid fraction format") index)
             (progn
               (setf denominator (parse-integer denom-str))
               (if (zerop denominator)
                   (values (list :error "Division by zero in fraction") index)
                   (values (list 'VALUEF (cons numerator denominator)) index))))))
      ;; Tam sayı
      (t
       (values (list 'VALUEI (parse-integer (subseq input start-index index))) 
               index)))))

(defun parse-identifier (input index)
  "Tanımlayıcıları ve özel kelimeleri parse eder"
  (let ((start-index index)
        (length (length input)))
    (loop
      (let ((char (if (< index length)
                     (char input index)
                     nil)))
        (if (and char (valid-identifier-char-p char))
            (setf index (1+ index))
            (return))))
    (let ((identifier (subseq input start-index index)))
      (cond
        ;; Boolean ve nil değerleri direkt döndürülür
        ((string= identifier "true")
         (values (list 'VALUEB t) index))
        ((string= identifier "false")
         (values (list 'VALUEB nil) index))
        ((string= identifier "nil")
         (values (list 'VALUENIL nil) index))
        ;; Keywords
        ((member identifier
                 '("and" "or" "not" "equal" "less" "list"
                   "append" "concat" "set" "defvar" "deffun" "for" "if" "while"
                   "exit" "load" "print")
                 :test #'string=)
         (values (intern (string-upcase (concatenate 'string "KW_" identifier))) 
                 index))
        ;; Normal tanımlayıcı
        ((every #'valid-identifier-char-p identifier)
         (values (list 'IDENTIFIER identifier) index))
        (t
         (values (list :error input) index))))))

(defun lexer (input)
  "Girdi satırını tokenize eder"
  (let ((result-tokens nil))
    (do ((index 0)
         (length (length input)))
        ((>= index length) (when result-tokens (nreverse result-tokens)))
      (let ((char (char input index)))
        (cond
          ((member char '(#\Space #\Tab #\Newline #\Return))
           (incf index))
          ((char= char #\")  ; string başlangıcı
           (multiple-value-bind (token new-index)
               (parse-string input index)
             (when token
               (push token result-tokens))
             (setf index new-index)))
          ((char= char #\')  ; char başlangıcı veya quote
           (if (and (< (+ index 1) length)
               (char= (char input (+ index 1)) #\())  ; quote işareti
               (progn
                 (push 'OP_QUOTE result-tokens)
                 (incf index))
               (multiple-value-bind (token new-index)  ; karakter literali
                   (parse-char input index)
                 (when token
                   (push token result-tokens))
                 (setf index new-index))))
          ((or (char-alpha-p char) (char= char #\_))
           (multiple-value-bind (token new-index)
               (parse-identifier input index)
             (when token
               (push token result-tokens))
             (setf index new-index)))
          ((char-digit-p char)
           (multiple-value-bind (token new-index)
               (parse-number input index)
             (when token
               (push token result-tokens))
             (setf index new-index)))
          ((member char '(#\+ #\- #\* #\/ #\( #\) #\,))
           (push (parse-operator char) result-tokens)
           (incf index))
          (t
           (return-from lexer (list (list :error (format nil "Invalid character: ~a" char))))))))))

;;; Parser Section
;;; Parser Section
;;; Parser Section
;;; Parser Section
;;; Parser Section
;;; Parser Section
;;; Parser Section
(defstruct parser-state
  (tokens nil)
  (current-index 0)
  (errors nil))

(defun current-token (state)
  "Mevcut token'ı döndürür"
  (when (< (parser-state-current-index state)
           (length (parser-state-tokens state)))
    (nth (parser-state-current-index state)
         (parser-state-tokens state))))

(defun advance-token (state)
  "Bir sonraki token'a geçer"
  (incf (parser-state-current-index state)))

(defun expect-token (state expected-token)
  "Beklenen token'ı kontrol eder"
  (let ((current (current-token state)))
    (if (eq current expected-token)
        (progn (advance-token state) t)
        (progn 
          (push (format nil "Expected ~A but got ~A" expected-token current)
                (parser-state-errors state))
          nil))))

(defun parse-list (state)
  "Liste yapısını parse eder"
  (when (eq (current-token state) 'OP_QUOTE)
    (advance-token state)                   ; quote'u atla
    (if (eq (current-token state) 'OP_OP)  ; açık parantez kontrol
        (progn
          (advance-token state)            ; açık parantezi atla
          (let ((values (parse-list-values state)))
            (when (eq (current-token state) 'OP_CP)
              (advance-token state)         ; kapalı parantezi atla
              (list 'LIST_VALUE values))))
        (push "Expected opening parenthesis after quote" 
              (parser-state-errors state)))))

(defun parse-list-values (state)
  "Liste değerlerini parse eder"
  (let ((values nil))
    (loop until (eq (current-token state) 'OP_CP) do
          (let ((value (parse-exp state)))
            (when value
              (push value values))
            (when (eq (current-token state) 'OP_COMMA)
              (advance-token state))))
    (nreverse values)))

(defun parse-assignment (state)
  "Değişken atamasını parse eder"
  (advance-token state) ; Skip 'set'
  (let ((var-token (current-token state)))
    (if (and (listp var-token)
             (eq (first var-token) 'IDENTIFIER))
        (progn
          (advance-token state)
          (let ((value-exp (parse-exp state)))
            (if (expect-token state 'OP_CP)  ; Kapanış parantezini bekle
                (if (parser-state-errors state)
                    nil
                    (list 'set (second var-token) value-exp))
                (progn 
                  (push "Missing closing parenthesis in set expression"
                        (parser-state-errors state))
                  nil))))
        (progn
          (push "Expected identifier in assignment"
                (parser-state-errors state))
          nil))))

(defun parse-defvar (state)
  "Değişken tanımlamasını parse eder"
  (advance-token state) ; Skip 'defvar'
  (let ((var-token (current-token state)))
    (if (and (listp var-token)
             (eq (first var-token) 'IDENTIFIER))
        (progn
          (advance-token state)
          (let ((value-exp (parse-exp state)))
            (if (expect-token state 'OP_CP)  ; Kapanış parantezini bekle
                (if (parser-state-errors state)
                    nil
                    (list 'defvar (second var-token) value-exp))
                (progn 
                  (push "Missing closing parenthesis in defvar expression"
                        (parser-state-errors state))
                  nil))))
        (progn
          (push "Expected identifier in variable declaration"
                (parser-state-errors state))
          nil))))

(defun parse-if (state)
  "If ifadelerini parse eder"
  (advance-token state)  ; Skip 'if'
  (let ((condition (parse-exp state)))
    (when condition
      (let ((true-branch (parse-exp state)))
        (when true-branch
          (let ((false-branch (parse-exp state)))
            (when (and (or false-branch (eq (current-token state) 'OP_CP))
                      (expect-token state 'OP_CP))
              (list 'KW_IF condition true-branch false-branch))))))))

(defun parse-while (state)
  "Parse while loops"
  (advance-token state)  ; Skip 'while'
  (when (eq (current-token state) 'OP_OP)
    (advance-token state)
    (let ((condition (parse-exp state)))
      (when (eq (current-token state) 'OP_CP)
        (advance-token state)
        (let ((body (parse-exp state)))
          (if (expect-token state 'OP_CP)
              (list 'while condition body)
              (push "Missing closing parenthesis in while statement"
                    (parser-state-errors state))))))))

(defun parse-for (state)
  "For döngüsünü parse eder"
  (advance-token state)  ; Skip 'for'
  (when (eq (current-token state) 'OP_OP)
    (advance-token state)
    (let* ((var-token (current-token state))
           (var (when (and (listp var-token)
                          (eq (first var-token) 'IDENTIFIER))
                  (second var-token))))
      (when var
        (advance-token state)  ; Değişken ismini atla
        (let ((start (parse-exp state)))
          (when start
            (let ((end (parse-exp state)))
              (when end
                (if (eq (current-token state) 'OP_CP)
                    (progn
                      (advance-token state)  ; Kapanan parantezi atla
                      (let ((body (parse-exp state)))
                        (when body
                          (if (expect-token state 'OP_CP)
                              (list 'KW_FOR var start end body)
                              nil))))
                    nil)))))))))

(defvar *loop-variables* (make-hash-table :test #'equal))

(defun parse-function-def (state)
  "Fonksiyon tanımlarını parse eder"
  (advance-token state)  ; Skip 'deffun'
  (let ((name-token (current-token state)))
    (if (and (listp name-token)
             (eq (first name-token) 'IDENTIFIER))
        (progn
          (advance-token state)
          (if (eq (current-token state) 'OP_OP)
              (progn
                (advance-token state)
                (let ((params (parse-params state)))
                  (when params
                    (let ((body (parse-exp state)))
                      (when body
                        (if (expect-token state 'OP_CP)
                            (list 'KW_DEFFUN (second name-token) params body)
                            (push "Missing closing parenthesis in function definition"
                                  (parser-state-errors state))))))))
              (push "Expected opening parenthesis after function name"
                    (parser-state-errors state))))
        (push "Expected function name" (parser-state-errors state)))))
  
  (defun parse-function-call (state)
  "Fonksiyon çağrılarını parse eder"
  (let ((func-name (current-token state)))
    (when (and (listp func-name)
               (eq (first func-name) 'IDENTIFIER))
      (let ((name (second func-name)))
        (advance-token state)
        (let ((args nil))
          (loop until (eq (current-token state) 'OP_CP) do
                (let ((arg (parse-exp state)))
                  (when arg
                    (push arg args))))
          (if (expect-token state 'OP_CP)
              (cons name (nreverse args))
              (push "Missing closing parenthesis in function call"
                    (parser-state-errors state))))))))

(defun parse-params (state)
  "Parse function parameters"
  (let ((params nil))
    (loop until (eq (current-token state) 'OP_CP) do
          (let ((param-token (current-token state)))
            (if (and (listp param-token)
                     (eq (first param-token) 'IDENTIFIER))
                (progn
                  (push (second param-token) params)
                  (advance-token state)
                  (when (eq (current-token state) 'OP_COMMA)
                    (advance-token state)))
                (return-from parse-params nil))))
    (advance-token state)  ; Kapanış parantezini atla
    (nreverse params)))

(defun parse-boolean-exp (state)
  "Boolean expression'ları parse eder"
  (let ((operator (current-token state)))
    (advance-token state)  ; operatörü ilerlet
    (case operator
      ((KW_AND KW_OR KW_EQUAL KW_LESS)  ; İki operandlı
       (let ((left (parse-exp state))
             (right (parse-exp state)))
         (when (and left right)
           (if (expect-token state 'OP_CP)
               (list operator left right)  ; operator'ı düzgün şekilde dön
               nil))))
      
      (KW_NOT  ; Tek operandlı
       (let ((operand (parse-exp state)))
         (if (expect-token state 'OP_CP)
             (list operator operand)
             nil)))
      
      (otherwise
       (push "Expected boolean operator" (parser-state-errors state))
       nil))))

(defun parse-exp (state)
  "Expression'ları parse eder"
  (let ((token (current-token state)))
    (cond
      ((eq token 'OP_OP)
       (advance-token state)
       (parse-compound-exp state))
      
      ((eq token 'OP_QUOTE)
       (parse-list state))
      
      ((and (listp token) 
            (eq (first token) 'IDENTIFIER))
       (advance-token state)
      
       (if (get-loop-var token)
           (list 'loop-var (second token))  ; Döngü değişkeni
           (list 'var (second token))))     ; Normal değişken
      
      ((and (listp token)
            (member (first token) 
                   '(VALUEI VALUEF VALUEB VALUENIL VALUESTR VALUECHAR LIST_VALUE)))
       (advance-token state)
       (list (first token) (second token)))
      
      (t (push "Invalid expression" 
               (parser-state-errors state))
         nil))))


(defun parse-compound-exp (state)
  "Bileşik ifadeleri parse eder"
  (let ((operator (current-token state)))
    (case operator
      ((KW_AND KW_OR KW_NOT KW_EQUAL KW_LESS)  ; Boolean operatörleri
       (parse-boolean-exp state))
      
      (KW_SET   ; Değişken atama
       (parse-assignment state))
      
      (KW_DEFVAR  ; Değişken tanımlama
       (parse-defvar state))
      
      (KW_IF     ; If ifadesi
       (parse-if state))

      (KW_PRINT  ; Print ifadesi
       (parse-print state))
      
      (KW_WHILE  ; While döngüsü
       (parse-while state))
      
      (KW_FOR    ; For döngüsü
       (parse-for state))
      
      (KW_DEFFUN ; Fonksiyon tanımı
       (parse-function-def state))
      
      ((OP_PLUS OP_MINUS OP_MULT OP_DIV)  ; Aritmetik operatörler
       (advance-token state)
       (let ((left (parse-exp state))
             (right (parse-exp state)))
         (if (expect-token state 'OP_CP)
             (list operator left right)
             nil)))
      
      (otherwise
        (if (and (listp operator)
                 (eq (first operator) 'IDENTIFIER))
            (parse-function-call state)
            (progn
              (push "Unknown operator in compound expression" 
                    (parser-state-errors state))
              nil))))))


 (defun parse-print (state)
  "Print ifadelerini parse eder"
  (advance-token state)  ; Skip 'print'
  (let ((value (parse-exp state)))
    (when value
      (if (expect-token state 'OP_CP)
          (list 'KW_PRINT value)
          (push "Missing closing parenthesis in print statement"
                (parser-state-errors state))))))




;;; Evaluator Section
;;; Evaluator Section
;;; Evaluator Section
;;; Evaluator Section
;;; Evaluator Section
;;; Evaluator Section
;;; Evaluator Section
(defstruct environment
  (variables (make-hash-table :test #'equal))
  (parent nil))

(defstruct gpp-value
  type
  value)

(defstruct function-def
  name
  params
  body)

(defun print-value (value)
  "Değeri yazdırır ve değeri döndürür"
  (format t "~A" (if (stringp value) value (format nil "~A" value)))
  value)


(defun create-function-environment (func args env)
  "Fonksiyon için yeni bir çevre oluşturur"
  (let ((new-env (make-new-scope env)))
    ;; Parametreleri yeni çevreye bağla
    (loop for param in (function-def-params func)
          for arg in args
          do (set-variable new-env param arg))
    new-env))

(defvar *function-table* (make-hash-table :test #'equal))

(defun get-loop-var (var-token)
  "Verilen token'ın döngü değişkeni olup olmadığını kontrol eder"
  (and (listp var-token)
       (eq (first var-token) 'IDENTIFIER)
       (gethash (second var-token) *loop-variables*)))

(defun make-new-scope (parent-env)
  "Yeni bir kapsam oluşturur"
  (make-environment :parent parent-env))

(defun set-variable (env name value)
  "Değişken değerini atar"
  (setf (gethash name (environment-variables env)) value)
  value)

(defun get-variable (env name)
  "Değişken değerini alır"
  (let ((value (gethash name (environment-variables env))))
    (if value
        value
        (if (environment-parent env)
            (get-variable (environment-parent env) name)
            (if (gethash name *loop-variables*)
                (error "Loop variable not initialized: ~A" name)
                (error "Undefined variable: ~A" name))))))



(defun evaluate-exp (exp env)
  "İfadeleri değerlendirir"
  (cond
    ((atom exp) exp)
    ((null exp) nil)
    ((listp exp)
     (case (first exp)
       ;; Değişken işlemleri
       (set
        (let* ((name (second exp))
               (value (evaluate-exp (third exp) env))
               (current-env (find-variable-environment env name)))
          (if current-env
              (progn
                (set-variable current-env name value)
                value)
              (error "Variable ~A not declared" name))))
       
       (defvar
        (let* ((name (second exp))
               (exists (gethash name (environment-variables env)))
               (value (evaluate-exp (third exp) env)))
          (if exists
              (error "Variable ~A already declared" name)
              (progn
                (set-variable env name value)
                value))))
       
       (var
        (get-variable env (second exp)))

       ;; Kontrol yapıları - if yapısı için özel değerlendirme
        (KW_IF
        (let ((condition-result (evaluate-exp (second exp) env)))
          (if condition-result
              (evaluate-exp (third exp) env)  ; true branch
              (when (fourth exp)
                (evaluate-exp (fourth exp) env)))))  ; false branch

       ;; Print işlemi
       (KW_PRINT
        (let ((value (evaluate-exp (second exp) env)))
          (format t "~A~%" value)
          nil))  ; print sadece nil döndürür

       ;; Boolean operatörleri
        ((KW_AND KW_OR KW_EQUAL KW_LESS)
        (let ((left (evaluate-exp (second exp) env))
              (right (evaluate-exp (third exp) env)))
          (case (first exp)
            (KW_LESS 
             (if (and (numberp left) (numberp right))
                 (< left right)
                 (error "LESS operator requires numeric operands")))
            (KW_AND (and left right))
            (KW_OR (or left right))
            (KW_EQUAL (equal left right)))))
       
       (KW_NOT
        (not (evaluate-exp (second exp) env)))

       ;; Aritmetik operatörler
       ((OP_PLUS OP_MINUS OP_MULT OP_DIV)
        (let ((left (evaluate-exp (second exp) env))
              (right (evaluate-exp (third exp) env)))
          (evaluate-arithmetic-op (first exp) left right)))

       ;; Fonksiyon tanımlama
       (KW_DEFFUN
        (let ((name (second exp))
              (params (third exp))
              (body (fourth exp)))
          (setf (gethash name *function-table*)
                (make-function-def :name name
                                 :params params
                                 :body body))
          name))

       ;; Değerler
       ((VALUEI VALUEF VALUEB VALUENIL VALUESTR VALUECHAR LIST_VALUE)
        (second exp))

        
    (KW_FOR
        (let* ((var (second exp))
               (start (evaluate-exp (third exp) env))
               (end (evaluate-exp (fourth exp) env))
               (body (fifth exp))
               (loop-env (make-new-scope env)))
          
          ;; Döngü değişkenini otomatik olarak tanımla
          (set-variable loop-env var start)
          
          ;; Döngüyü çalıştır
          (loop for i from start to end do
                (set-variable loop-env var i)
                (evaluate-exp body loop-env))
          nil))  ; For döngüsü nil döndürür
       
       (var
        (if (gethash (second exp) *loop-variables*)
            (get-variable env (second exp))  ; Döngü değişkeni
            (get-variable env (second exp)))) ; Normal değişken

       ;; Loop değişkeni erişimi için özel durum
       (loop-var
        (get-variable env (second exp)))

       ;; Diğer durumlar - basit fonksiyon çağrıları
       (otherwise
        (if (gethash (first exp) *function-table*)
            (let* ((func (gethash (first exp) *function-table*))
                   (args (mapcar #'(lambda (arg) (evaluate-exp arg env)) 
                               (rest exp)))
                   (func-env (make-new-scope env)))
              ;; Fonksiyon parametrelerini bağla
              (loop for param in (function-def-params func)
                    for arg in args
                    do (set-variable func-env param arg))
              ;; Fonksiyon gövdesini değerlendir
              (evaluate-exp (function-def-body func) func-env))
            (error "Unknown expression type: ~A" exp)))))
    (t exp)))



(defun evaluate-arithmetic-op (op left right)
  "Aritmetik operasyonları değerlendirir"
  (case op
    (OP_PLUS (+ left right))
    (OP_MINUS (- left right))
    (OP_MULT (* left right))
    (OP_DIV (/ left right))))

(defun find-variable-environment (env name)
  "Değişkenin tanımlı olduğu çevreyi bulur"
  (if (gethash name *loop-variables*)
      env  ; Döngü değişkeni için mevcut çevreyi kullan
      (loop for current-env = env then (environment-parent current-env)
            while current-env
            when (gethash name (environment-variables current-env))
            return current-env)))

(defun evaluate-function-call (name args env)
  "Fonksiyon çağrılarını değerlendirir"
  (let ((func (gethash name *function-table*)))
    (if func
        (let ((new-env (make-new-scope env)))
          ;; Parametreleri bağla
          (loop for param in (function-def-params func)
                for arg in (mapcar #'(lambda (x) (evaluate-exp x env)) args)
                do (set-variable new-env param arg))
          ;; Fonksiyon gövdesini yeni ortamda değerlendir
          (evaluate-exp (function-def-body func) new-env))
        (error "Undefined function: ~A" name))))

;;; Main Interpreter
(defun process-line (line env)
  "Bir satırı işler"
  (let* ((tokens (lexer line))
         (parser-state (make-parser-state :tokens tokens)))
    (if (null tokens)
        nil
        (handler-case
            (let ((parse-tree (parse-exp parser-state)))
              (when parse-tree
                (if (parser-state-errors parser-state)
                    (format t "Syntax Error: ~{~A~^, ~}~%" 
                            (parser-state-errors parser-state))
                    (let ((result (evaluate-exp parse-tree env)))
                      ;; Sadece sonuç nil değilse ve print işlemi değilse yazdır
                      (when (and result 
                               (not (eq (first parse-tree) 'KW_PRINT)))
                        (format t "~A~%" result))))))
          (error (e)
            (format t "Error: ~A~%" e))))))


(defun read-until-balanced ()
  "Dengeli parantezleri olan bir girdi okur"
  (format t "> ") ; prompt
  (force-output)
  (let ((input (read-line nil nil)))
    (when input
      (string-trim '(#\Space #\Tab #\Newline #\Return) input))))

(defun gppinterpreter (&optional filename)
  "G++ Interpreter'ı başlatır"
  (let ((env (make-environment)))
    (if filename
        ;; File mode
        (with-open-file (stream filename)
          (loop for line = (read-line stream nil)
                while line do
                (process-line line env)))
        ;; Interactive mode
        (loop
          (let ((input (read-until-balanced)))
            (when (or (null input)
                      (string-equal input "exit"))
              (return))
            (process-line input env))))))



;; Karakterin harf olup olmadığını kontrol eden fonksiyon
(defun char-alpha-p (char)
  "Karakterin harf olup olmadığını kontrol eder."
  (and char
       (or (char<= #\A char #\Z)
           (char<= #\a char #\z))))

;; Karakterin rakam olup olmadığını kontrol eden fonksiyon
(defun char-digit-p (char)
  "Karakterin rakam olup olmadığını kontrol eder."
  (and char
       (char<= #\0 char #\9)))

;; Tanımlayıcıdaki karakterlerin geçerli olup olmadığını kontrol eden fonksiyon
(defun valid-identifier-char-p (char)
  "Tanımlayıcı içinde kullanılabilecek karakterleri kontrol eder."
  (or (char-alpha-p char) (char-digit-p char) (char= char #\_)))

;; Operatörleri tanıyan fonksiyon
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
    (#\= 'OP_ASSIGN)))

;; Sayıları tanıyan fonksiyon
(defun parse-number (input index)
  "Tamsayıları ve kesirli sayıları tanır. Sayıdan sonra harf veya '_' geliyorsa geçersiz token olarak işaretler."
  (let ((start-index index)
        (length (length input))
        (state 'integer))
    ;; DFA: number durumu
    ;; Rakamları ve kesirli sayıları okur
    (loop
      (let ((char (if (< index length)
                      (char input index)
                      nil)))
        (cond
          ;; Rakam ise devam et
          ((and char (char-digit-p char))
           (setf index (1+ index)))
          ;; Kesir ayıracı ':' veya 'f' ise kesir moduna geç
          ((and char (or (char= char #\:) (char= char #\f)))
           (if (eq state 'integer)
               (progn
                 (setf state 'fraction)
                 (setf index (1+ index)))
               (return)))
          ;; Sayının sonu
          (t (return)))))
    ;; Kontrol: Sayının ardından gelen karakter harf veya '_' ise hata
    (let ((next-char (if (< index length)
                         (char input index)
                         nil)))
      (cond
        ;; Kesirli sayı
        ((eq state 'fraction)
         (values (list 'VALUEF (subseq input start-index index)) index))
        ;; Sayının ardından harf veya '_' geliyorsa geçersiz token
        ((and next-char (or (char-alpha-p next-char) (char= next-char #\_)))
         ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
         (return-from parse-number (values (list :error input) index)))
        ;; Geçerli tamsayı
        (t
         (values (list 'VALUEI (subseq input start-index index)) index))))))

;; Tanımlayıcıları ve anahtar kelimeleri tanıyan fonksiyon
(defun parse-identifier (input index)
  "Tanımlayıcıları ve anahtar kelimeleri tanır."
  (let ((start-index index)
        (length (length input)))
    ;; DFA: identifier durumu
    ;; Tanımlayıcı veya anahtar kelimeyi okur
    (loop
      (let ((char (if (< index length)
                      (char input index)
                      nil)))
        (if (and char (valid-identifier-char-p char))
            (setf index (1+ index))
            (return))))
    (let ((identifier (subseq input start-index index)))
      (cond
        ;; Anahtar kelime kontrolü (büyük/küçük harf duyarlı)
        ((member identifier
                 '("and" "or" "not" "equal" "less" "nil" "list"
                   "append" "concat" "set" "deffun" "for" "if"
                   "exit" "load" "print" "true" "false")
                 :test 'string=)
         ;; Anahtar kelimeyi KW_ prefixiyle intern ederek döndür
         (values (intern (string-upcase (concatenate 'string "KW_" identifier))) index))
        ;; Geçerli tanımlayıcı
        ((every #'valid-identifier-char-p identifier)
         ;; Tanımlayıcı olarak döndür
         (values (list 'IDENTIFIER identifier) index))
        ;; Geçersiz tanımlayıcı
        (t
         ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
         (return-from parse-identifier (values (list :error input) index)))))))

;; Lexer fonksiyonu, girdi satırını token'lara ayırır
(defun lexer (input)
  "Girdi satırını token'lara ayırır, DFA yapısını kullanarak."
  (let ((tokens '())
        (length (length input))
        (index 0)
        (state 'start))
    ;; DFA'nın durumları ve geçişleri
    ;; state değişkeni, mevcut durumu tutar
    (labels ((peek ()
               (if (< index length)
                   (char input index)
                   nil))
             (peek-next ()
               (if (< (1+ index) length)
                   (char input (1+ index))
                   nil))
             (advance ()
               (prog1 (peek)
                 (setf index (1+ index)))))
      (loop while (< index length) do
           (let ((char (peek)))
             (let ((next-char (peek-next)))
               (cond
                 ;; DFA durumları ve geçişleri
                 ((eq state 'start)
                  ;; Başlangıç durumu
                  (cond
                    ;; Boşluk karakterleri
                    ((member char '(#\Space #\Tab #\Newline #\Return))
                     (advance))
                    ;; Yorum satırları (';;' ile başlayan)
                    ((and (char= char #\;) next-char (char= next-char #\;))
                     ;; Yorum satırı tespit edildi
                     (loop do (advance)
                           while (and (peek) (not (char= (peek) #\Newline))))
                     ;; Satır sonuna veya input bitimine gelene kadar ilerler
                     (advance)
                     ;; COMMENT tokenı
                     (push 'COMMENT tokens)
                     (setf state 'start))
                    ;; Harf veya '_' ile başlayan tanımlayıcılar
                    ((or (char-alpha-p char) (char= char #\_))
                     (setf state 'identifier))
                    ;; Sayılar
                    ((char-digit-p char)
                     (setf state 'number))
                    ;; Operatörler, parantezler ve atama operatörleri
                    ((member char '(#\+ #\- #\* #\/ #\( #\) #\, #\=))
                     (setf state 'operator))
                    ;; Tanınmayan karakterler
                    (t
                     ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
                     (return-from lexer (list (list :error input))))))
                 ;; Tanımlayıcı durumu
                 ((eq state 'identifier)
                  ;; DFA: identifier durumu
                  (multiple-value-bind (result new-index)
                      (parse-identifier input index)
                    (setf index new-index)
                    (cond
                      ((and (listp result) (eq (first result) :error))
                       ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
                       (return-from lexer (list (list :error input))))
                      (t
                       (push result tokens)
                       (setf state 'start)))))
                 ;; Sayı durumu
                 ((eq state 'number)
                  ;; DFA: number durumu
                  (multiple-value-bind (result new-index)
                      (parse-number input index)
                    (setf index new-index)
                    (cond
                      ((and (listp result) (eq (first result) :error))
                       ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
                       (return-from lexer (list (list :error input))))
                      (t
                       (push result tokens)
                       (setf state 'start)))))
                 ;; Operatör durumu
                 ((eq state 'operator)
                  ;; DFA: operator durumu
                  (push (parse-operator char) tokens)
                  (advance)
                  (setf state 'start))
                 ;; Hata durumu
                 ((eq state 'error)
                  ;; Hata durumuna geç ve tüm satırı hata olarak değerlendir
                  (return-from lexer (list (list :error input)))))))))
    ;; Eğer tokens listesi boş değilse, token'ları döndür
    (if (null tokens)
        nil
        (nreverse tokens))))

;; Satırı işleyen fonksiyon, lexer'dan dönen tokenları işler
(defun process-line (line)
  "Bir satırı lexer'a gönderir ve dönen token'ları işler."
  (let ((tokens (lexer line)))
    (cond
      ((null tokens)
       ;; Lexer hiçbir token döndürmediyse, satır boş veya sadece boşluk içeriyor demektir.
       ;; Bu durumda hiçbir şey yapma.
       nil)
      ((and (listp tokens)
            (listp (first tokens))
            (eq (first (first tokens)) :error))
       ;; Lexer tüm satırı hata olarak döndürür
       (format t "SYNTAX_ERROR: '~a' cannot be tokenized.~%" line))
      (t
       (dolist (token tokens)
         (format t "~a~%" token))))))

;; GPP Interpreter'ı başlatan fonksiyon
(defun gppinterpreter (&optional filename)
  "GPP Interpreter'ı başlatır. İsteğe bağlı olarak bir dosya adı alabilir."
  (if filename
      ;; Dosya okuma modu
      (with-open-file (stream filename)
        (loop for line = (read-line stream nil)
              while line do
                (process-line line)))
      ;; Kullanıcı girişi modu
      (loop
        (format t "> ")
        (let ((line (read-line *standard-input* nil)))
          (when line
            (if (string= (string-downcase line) "quit")
                (progn
                  (format t "Program sonlandırılıyor...\n")
                  (quit))
                (process-line line)))))))

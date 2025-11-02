;;;; loader.lisp
;; Ana kod yükleyici ve başlatıcı

;; Compile-time warningları bastır
(declaim (sb-ext:muffle-conditions style-warning))

;; Forward declarations - Tüm fonksiyonları önceden bildir
(declaim (ftype (function (character) boolean) char-alpha-p))
(declaim (ftype (function (character) boolean) char-digit-p))
(declaim (ftype (function (character) boolean) valid-identifier-char-p))
(declaim (ftype (function (character) symbol) parse-operator))
(declaim (ftype (function (string fixnum) t) parse-number))
(declaim (ftype (function (string fixnum) t) parse-identifier))
(declaim (ftype (function (string) list) lexer))              ; lexer'ın deklarasyonunu ekledik
(declaim (ftype (function (t) t) current-token))
(declaim (ftype (function (t) t) advance-token))
(declaim (ftype (function (t t) t) expect-token))
(declaim (ftype (function (parser-state) t) parse-exp))
(declaim (ftype (function (parser-state) t) parse-compound-exp))
(declaim (ftype (function (parser-state) t) parse-function-def))
(declaim (ftype (function (environment string t) t) set-variable))
(declaim (ftype (function (environment string) t) get-variable))
(declaim (ftype (function (t environment) t) evaluate-exp))
(declaim (ftype (function (string environment) t) process-line))
(declaim (ftype (function (&optional string) t) gppinterpreter))

;; Ana kodu yükle
(load "gpp_interpreter.lisp")

;; SBCL başlangıç mesajını bastır
(setf sb-ext:*muffled-warnings* 'style-warning)

;; Interpreter'ı başlat
(format t "~%G++ Interpreter başlatılıyor...~%")
(format t "Çıkmak için 'exit' yazın.~%~%")

;; Command line argümanlarını al
(defvar *command-line-args* (rest sb-ext:*posix-argv*))

;; Eğer dosya adı verilmişse onu kullan, yoksa interaktif mod
(if *command-line-args*
    (gppinterpreter (first *command-line-args*))
    (gppinterpreter))

;; SBCL'den çık
(sb-ext:quit)

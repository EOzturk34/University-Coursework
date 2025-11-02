(defun read-file (filename)
  "Reads a file and returns its content line by line as a list."
  (with-open-file (stream filename)
    (labels ((read-lines ()
               (let ((line (read-line stream nil)))
                 (if line
                     (cons line (read-lines))
                     nil))))
      (read-lines))))

(defun write-file (filename content)
  "Writes the content to a file, where content is a list of strings."
  (with-open-file (stream filename
                          :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (mapc (lambda (line) (format stream "~a~%" line)) content)))

(defun split-string (line)
  "Splits a line into tokens, keeping strings enclosed in double quotes together."
  (labels ((split-chars (chars current-token result in-string)
             (if (null chars)
                 (if (string= current-token "")
                     (reverse result)
                     (reverse (cons (string-trim " " current-token) result)))
                 (let ((char (car chars)))
                   (cond
                    ((char= char #\")
                     (split-chars (cdr chars)
                                  (concatenate 'string current-token (string char))
                                  result
                                  (not in-string)))
                    (in-string
                     (split-chars (cdr chars)
                                  (concatenate 'string current-token (string char))
                                  result
                                  in-string))
                    ((find char '(#\Space #\( #\) #\; #\, #\{))
                     (if (string= current-token "")
                         (split-chars (cdr chars) "" result in-string)
                         (split-chars (cdr chars) "" (cons (string-trim " " current-token) result) in-string)))
                    (t
                     (split-chars (cdr chars)
                                  (concatenate 'string current-token (string char))
                                  result
                                  in-string)))))))
    (split-chars (coerce line 'list) "" '() nil)))

(defun simple-param-conversion (param-type)
  "Converts simple parameter types from C to Lisp types."
  (cond ((string= param-type "int") "integer")
        ((string= param-type "char") "character")
        ((string= param-type "double") "double-float")
        ((string= param-type "float") "single-float")
        ((string= param-type "void") "nil")
        (t nil)))

(defun convert-closing-brace ()
  "Converts a C closing brace to a Lisp closing parenthesis with a newline."
  (concatenate 'string ")" (string #\Newline)))

(defun collect-param-names (params)
  "Recursively collects parameter names from a list of tokens."
  (if (or (null params) (null (cdr params)))
      nil
      (cons (second params)
            (collect-param-names (cddr params)))))

(defun convert-function-definition (line)
  "Converts a C function definition line to a Lisp defun form."
  (let* ((tokens (split-string line))
         (func-name (second tokens))
         (params (subseq tokens 2))
         (param-names (collect-param-names params)))
    (format nil "(defun ~a (~{~a~^ ~})" func-name param-names)))

(defun convert-main-function ()
  "Converts a C main function definition line to a Lisp defun form with let block."
  (format nil "(defun main ()~%  (let ("))

(defun convert-function-prototype (line)
  "Converts a C function prototype to a Lisp declaim form."
  (let* ((tokens (split-string line))
         (return-type (simple-param-conversion (first tokens)))
         (func-name (second tokens))
         (params (subseq tokens 2))
         (lisp-params (remove nil (mapcar #'simple-param-conversion params))))
    (format nil "(declaim (ftype (function (~{~a~^ ~}) ~a) ~a))~%"
            lisp-params return-type func-name)))

(defun convert-return-arithmetic (line)
  "Converts a return arithmetic statement from C to Lisp."
  (let* ((tokens (split-string line))
         (left-side (second tokens))
         (operator (third tokens))
         (right-side (fourth tokens)))
    (format nil "  (~a ~a ~a)" operator left-side right-side)))

(defun convert-return-simple (line)
  "Converts a simple return statement (e.g., 'return a;') to Lisp format '(return a)'."
  (let* ((tokens (split-string line))
         (return-value (second tokens)))
    (format nil "  (~a)" return-value)))

(defun convert-assignment (line)
  "Converts an assignment line (either simple or function call) from C to Lisp."
  (let* ((tokens (split-string line))
         (left-side (second tokens))
         (func-name (fourth tokens))
         (param-tokens (subseq tokens 4)))
    (if (find #\( line)
        (format nil "    (~a (~a ~{~a~^ ~}    ))" left-side func-name
                (mapcar (lambda (token) (string-trim " " token)) param-tokens))
        (format nil "    (~a ~a)" left-side (fourth tokens)))))

(defun convert-for-loop (line)
  "Converts a C for-loop to a Lisp loop form."
  (let* ((tokens (split-string line)))
    (cond
     ((and (equal (seventh tokens) "<") (equal (ninth tokens) "i++"))
      (format nil "    (loop for ~a from ~a below ~a do" (third tokens) (fifth tokens) (eighth tokens)))
     ((and (equal (seventh tokens) "<=") (equal (ninth tokens) "i++"))
      (format nil "    (loop for ~a from ~a to ~a do" (third tokens) (fifth tokens) (eighth tokens)))
     ((and (equal (seventh tokens) ">") (equal (ninth tokens) "i--"))
      (format nil "    (loop for ~a from ~a downto ~a while (> ~a ~a) do"
              (third tokens) (fifth tokens) (eighth tokens) (third tokens) (eighth tokens)))
     ((and (equal (seventh tokens) ">=") (equal (ninth tokens) "i--"))
      (format nil "    (loop for ~a from ~a downto ~a do" (third tokens) (fifth tokens) (eighth tokens)))
     (t (format nil "    ; Unsupported for-loop format")))))

(defun convert-while-loop (line)
  "Converts a C while-loop to a Lisp loop form."
  (let* ((tokens (split-string line)))
    (cond
     ((equal (third tokens) "<")
      (format nil "    (loop while (< ~a ~a) do" (second tokens) (fourth tokens)))
     ((equal (third tokens) "<=")
      (format nil "    (loop while (<= ~a ~a) do" (second tokens) (fourth tokens)))
     ((equal (third tokens) ">")
      (format nil "    (loop while (> ~a ~a) do" (second tokens) (fourth tokens)))
     ((equal (third tokens) ">=")
      (format nil "    (loop while (>= ~a ~a) do" (second tokens) (fourth tokens)))
     ((equal (third tokens) "!=")
      (format nil "    (loop while (/= ~a ~a) do" (second tokens) (fourth tokens)))
     ((equal (third tokens) "==")
      (format nil "    (loop while (= ~a ~a) do" (second tokens) (fourth tokens)))
     (t (format nil "    ; Unsupported while-loop format")))))

(defun convert-if-statement (line)
  "Converts a C if-statement to a Lisp if form."
  (let* ((tokens (split-string line)))
    (cond
     ((equal (third tokens) "<")
      (format nil "    (if (< ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     ((equal (third tokens) "<=")
      (format nil "    (if (<= ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     ((equal (third tokens) ">")
      (format nil "    (if (> ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     ((equal (third tokens) ">=")
      (format nil "    (if (>= ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     ((equal (third tokens) "!=")
      (format nil "    (if (/= ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     ((equal (third tokens) "==")
      (format nil "    (if (= ~a ~a)~%      (progn" (second tokens) (fourth tokens)))
     (t (format nil "    ; Unsupported if format")))))

(defun replace-all (str old new)
  (let ((old-length (length old)))
    (labels ((helper (start)
               (let ((pos (search old str :start2 start)))
                 (if pos
                     (concatenate 'string
                                  (subseq str start pos)
                                  new
                                  (helper (+ pos old-length)))
                     (subseq str start)))))
      (helper 0))))

(defun replace-format-specifiers (format-string)
  "Replaces C-style format specifiers in a string with Lisp-style format specifiers."
  (replace-all
   (replace-all
    (replace-all
     (replace-all format-string "%d" "~d")
     "%f" "~f")
    "%s" "~a")
   "\\n" "~%"))

(defun remove-surrounding-quotes (str)
  "If STR starts and ends with double quotes, remove them."
  (if (and (>= (length str) 2)
           (char= (char str 0) #\")
           (char= (char str (- (length str) 1)) #\"))
      (subseq str 1 (- (length str) 1))
      str))

(defun convert-print-statement (line)
  "Converts a C printf statement to a Lisp format statement."
  (let* ((tokens (split-string line))
         (format-string (remove-surrounding-quotes (second tokens)))
         (lisp-format-string (replace-format-specifiers format-string))
         (args (subseq tokens 2 (1- (length tokens)))))
    (format nil "    (format t \"~a\" ~{~a~^ ~})" lisp-format-string args)))

(defun convert-variable-declaration (line)
  "Converts a variable declaration (e.g., 'int x;') to Lisp format (x nil)."
  (let* ((tokens (split-string line))
         (variable-name (second tokens)))
    (format nil "    (~a nil)" variable-name)))

(defun convert-simple-assignment (line)
  "Converts a simple C assignment (e.g., 'x = 5;') to Lisp format '(setf x 5)'."
  (let* ((tokens (split-string line))
         (variable (first tokens))
         (value (third tokens)))
    (format nil "    (setf ~a ~a)" variable value)))

(defun convert-assignment-arithmetic (line)
  "Converts an arithmetic assignment (e.g., 'f = a + b;') to Lisp format '(setf f (+ a b))'."
  (let* ((tokens (split-string line))
         (variable (first tokens))
         (operator (fourth tokens))
         (operand1 (third tokens))
         (operand2 (fifth tokens)))
    (format nil "    (setf ~a (~a ~a ~a))" variable operator operand1 operand2)))

(defun convert-notDeclaration-assignment-functionCall (line)
  "Converts a function call assignment (e.g., 'g = sum(x, y);') to Lisp format '(setf g (sum x y))'."
  (let* ((tokens (split-string line))
         (variable (first tokens))
         (function-name (third tokens))
         (args (subseq tokens 3 (1- (length tokens)))))
    (format nil "    (setf ~a (~a ~{~a~^ ~}))" variable function-name args)))

(defun convert-assignment-declaration-arithmetic (line)
  "Converts a declaration assignment with arithmetic (e.g., 'int h = g + f;') to Lisp format '(setf h (+ g f))'."
  (let* ((tokens (split-string line))
         (variable (second tokens))
         (operator (fifth tokens))
         (operand1 (fourth tokens))
         (operand2 (sixth tokens)))
    (format nil "    (setf ~a (~a ~a ~a))" variable operator operand1 operand2)))

(defun convert-increment (line)
  "Converts a postfix increment (e.g., 'i++;') to Lisp format '(incf i)'."
  (let* ((tokens (split-string line))
         (variable (subseq (first tokens) 0 (- (length (first tokens)) 2))))
    (format nil "    (incf ~a)" variable)))

(defun convert-decrement (line)
  "Converts a postfix decrement (e.g., 'i--;') to Lisp format '(decf i)'."
  (let* ((tokens (split-string line))
         (variable (subseq (first tokens) 0 (- (length (first tokens)) 2))))
    (format nil "    (decf ~a)" variable)))

(defun determine-line-type (line)
  "Determines the type of a given C line based on its content."
  (cond
    ((search "return" line)
     (if (search "0" line)
         'return-zero
         (if (some (lambda (op) (search op line)) '("+" "-" "*" "/" "%"))
             'return-arithmetic
             'return-simple)))
    ((search "}" line) 'closing-brace)
    ((search "for" line) 'for-loop)
    ((search "while" line) 'while-loop)
    ((search "if" line) 'if-statement)
    ((search "printf" line) 'print-statement)
    ((search "++" line) 'increment)
    ((search "--" line) 'decrement)
    ((and (search "=" line)
          (some (lambda (op) (search op line)) '("+" "-" "*" "/" "%"))
          (some (lambda (type) (search type line)) '("int" "float" "char" "double")))
     'assignment-declaration-arithmetic)
    ((and (search "=" line)
          (some (lambda (op) (search op line)) '("+" "-" "*" "/" "%"))
          (not (some (lambda (type) (search type line)) '("int" "float" "char" "double"))))
     'assignment-arithmetic)
    ((and (search "=" line)
          (not (search "(" line))
          (not (some (lambda (type) (search type line)) '("int" "float" "char" "double"))))
     'simple-assignment)
    ((and (search "=" line)
          (search "(" line)
          (search ")" line)
          (not (some (lambda (type) (search type line)) '("int" "float" "char" "double"))))
     'notDeclaration-assignment-functionCall)
    ((and (search "=" line) (not (search "(" line))) 'simple-assignment-declaration)
    ((and (search "=" line) (search "(" line)) 'assignment-function-call)
    ((search "main" line) 'main-function)
    ((and (some (lambda (type) (search type line)) '("int" "float" "char" "double"))
          (not (search "=" line))
          (not (search "(" line))
          (not (search ")" line)))
     'variable-declaration)
    ((some (lambda (type) (search type line)) '("int" "float" "char" "double" "void"))
     (if (and (search ";" line) (search "(" line))
         'function-prototype
         'function-definition))
    (t 'unknown)))

(defun process-line (line output-stream)
  "Processes a single C line based on its type and writes the converted line to the output stream."
  (let ((line-type (determine-line-type line)))
    (let ((converted (case line-type
                       (function-definition (convert-function-definition line))
                       (function-prototype (convert-function-prototype line))
                       (main-function (convert-main-function))
                       (return-arithmetic (convert-return-arithmetic line))
                       (return-simple (convert-return-simple line))
                       (closing-brace (convert-closing-brace))
                       (simple-assignment-declaration (convert-assignment line))
                       (assignment-declaration-arithmetic (convert-assignment-declaration-arithmetic line))
                       (notDeclaration-assignment-functionCall (convert-notDeclaration-assignment-functionCall line))
                       (increment (convert-increment line))  
                       (decrement (convert-decrement line))   
                       (simple-assignment (convert-simple-assignment line))
                       (assignment-arithmetic (convert-assignment-arithmetic line))
                       (for-loop (convert-for-loop line))
                       (while-loop (convert-while-loop line))
                       (if-statement (convert-if-statement line))
                       (print-statement (convert-print-statement line))
                       (assignment-function-call (convert-assignment line))
                       (variable-declaration (convert-variable-declaration line))
                       (return-zero "  )")
                       (t nil))))
      (when converted
        (format output-stream "~a~%" converted)))))

(defun process-file (input-file output-file)
  "Reads and converts a C file to Lisp by processing each line recursively."
  (with-open-file (input-stream input-file)
    (with-open-file (output-stream output-file
                                   :direction :output
                                   :if-exists :supersede
                                   :if-does-not-exist :create)
      (labels ((process-recursively ()
                 (let ((line (read-line input-stream nil)))
                   (when line
                     (process-line line output-stream)
                     (process-recursively)))))
        (process-recursively)))))


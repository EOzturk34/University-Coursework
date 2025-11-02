# Makefile for compiling gpp_lexer.l on Ubuntu

# Derleyici ve Flex tanımları
CC = gcc
LEX = flex

# Dosya isimleri
LEX_FILE = gpp_lexer.l
LEX_C_FILE = lex.yy.c
OUTPUT = lexer

# Kütüphaneler
LIBS = -lfl

# Phony hedefleri
.PHONY: all clean

# Varsayılan hedef
all: clean $(OUTPUT)

# Çıktı dosyasını oluşturma kuralı
$(OUTPUT): $(LEX_C_FILE)
	$(CC) -o $(OUTPUT) $(LEX_C_FILE) $(LIBS)

# Flex ile C kodunu üretme kuralı
$(LEX_C_FILE): $(LEX_FILE)
	$(LEX) $(LEX_FILE)

# Temizleme kuralı
clean:
	rm -f $(OUTPUT) $(LEX_C_FILE)


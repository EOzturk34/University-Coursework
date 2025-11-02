JC = javac
JFLAGS =
JD = javadoc
JDFLAGS = -protected -splitindex -use -author -version -d ./javadoc
RM = rm -f
JR = java

# Java kaynak dosyalarını ve oluşturulacak .class dosyalarının konumunu belirtin
CLASSES = \
        FileSystem.java \
        Directory.java \
        FileSystemElement.java \
        File.java \
        Main.java

# 'make' veya 'make all' çalıştırıldığında çağrılacak olan varsayılan hedef
all: classes doc run

# Sınıfları derleyen ve Main sınıfını çalıştıran hedefler
classes: $(CLASSES:.java=.class)

run: Main.class
	$(JR) Main

# Main sınıfını derleme
Main.class: Main.java
	$(JC) $(JFLAGS) Main.java

# Genel kural Java kaynak dosyalarını .class dosyalarına derlemek için
%.class: %.java
	$(JC) $(JFLAGS) $<

# Javadoc oluşturma
doc:
	$(JD) $(JDFLAGS) ./*.java

# Derlenmiş dosyaları temizleme
clean:
	$(RM) *.class

# Javadoc dosyalarını temizleme
cleandoc:
	$(RM) -r ./javadoc

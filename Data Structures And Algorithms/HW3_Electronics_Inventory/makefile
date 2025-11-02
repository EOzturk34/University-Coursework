JC = javac
JFLAGS = -classpath .
JD = javadoc
JDFLAGS = -protected -splitindex -use -author -version -d ./javadoc
RM = rm -f
JR = java

# Kaynak dosyalarının bulunduğu dizin
SRC_DIR = ./EmrePack

# Java kaynak dosyalarını ve oluşturulacak .class dosyalarının konumunu belirtin
CLASSES = \
        $(SRC_DIR)/Inventory.java \
        $(SRC_DIR)/Device.java \
        $(SRC_DIR)/SmartPhone.java \
        $(SRC_DIR)/Laptop.java \
        $(SRC_DIR)/SmartWatch.java \
        $(SRC_DIR)/Computer.java \
        $(SRC_DIR)/TV.java \
        Main.java

# 'make' veya 'make all' çalıştırıldığında çağrılacak olan varsayılan hedef
all: classes doc run

# Sınıfları derleyen ve Main sınıfını çalıştıran hedefler
classes: $(CLASSES:.java=.class)

run: Main.class
	$(JR) Main

Main.class:
	$(JC) $(JFLAGS) Main.java

# Genel kural Java kaynak dosyalarını .class dosyalarına derlemek için
%.class: %.java
	$(JC) $(JFLAGS) $<

# Javadoc oluşturma
doc:
	$(JD) $(JDFLAGS) $(SRC_DIR)/*.java

# Derlenmiş dosyaları temizleme
clean:
	$(RM) $(SRC_DIR)/*.class Main.class

# Javadoc dosyalarını temizleme
cleandoc:
	$(RM) -r ./javadoc

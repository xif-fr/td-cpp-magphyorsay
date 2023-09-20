# arguments pour l'etape de compilation : tous les avertissements
CPPFLAGS := -std=c++17 -O3 -Wall -Wextra
# arguments pour l'édition de liens, bibliothèques externes
LDFLAGS := -lm 
# nom du compilateur
CPP := g++

# cible par défaut
all: monprogramme

# cible monprogramme : compilation des .cpp (dépendances .o), édition des liens, puis exécution
monprogramme: main.o uneclasse.o
	$(CPP) -o monprogramme $^ $(LDFLAGS)
	./monprogramme

# compilation de main.cpp
main.o: main.cpp uneclasse.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# compilation de uneclasse.cpp
uneclasse.o: uneclasse.cpp uneclasse.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# nettoyage des fichiers objets
clean:
	rm *.o
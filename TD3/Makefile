CPPFLAGS := -Ofast -flto -Wall -std=c++2b
LDFLAGS := -Ofast -flto -lsfml-graphics -lsfml-window -lsfml-system
CPP := g++

all:
	echo Choisissez ising_carre ou ising_hexa

# Compilation de reseau.cpp
reseau_carre.o: reseau_carre.cpp reseau_carre.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)
reseau_hexa.o: reseau_hexa.cpp reseau_hexa.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Compilation de ising.cpp
ising_carre.o: ising.cpp ising.h reseau_carre.o
	$(CPP) -o $@ -c $< $(CPPFLAGS)
ising_hexa.o: ising.cpp ising.h reseau_hexa.o
	$(CPP) -o $@ -c $< $(CPPFLAGS) -DRESEAU_HEXAGONAL

# Compilation de main_ising.cpp
main_carre.o: main_ising.cpp ising.h reseau_carre.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)
main_hexa.o: main_ising.cpp ising.h reseau_hexa.h
	$(CPP) -o $@ -c $< $(CPPFLAGS) -DRESEAU_HEXAGONAL

# Édition de liens
ising_carre: main_carre.o ising_carre.o reseau_carre.o
	$(CPP) -o ising $^ $(LDFLAGS)
ising_hexa: main_hexa.o ising_hexa.o reseau_hexa.o
	$(CPP) -o ising $^ $(LDFLAGS)

# Nettoyage des fichiers objets
clean:
	rm *.o
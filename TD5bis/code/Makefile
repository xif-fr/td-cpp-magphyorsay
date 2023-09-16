CPPFLAGS := -std=c++17 -O3 -Wall
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -lm -lfmt
CPP := g++

all:
	echo Choisissez billard_sec532, billard_sec533, billard_sec541, billard_sec542 ou billard_sec543

# Compilation de Util.cpp
Util.o: Util.cpp Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Compilation de ObjetReflechissantBase.cpp
ObjetReflechissantBase.o: ObjetReflechissantBase.cpp ObjetReflechissantBase.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Compilation de ObjetReflechissantLigne.cpp
ObjetReflechissantLigne.o: ObjetReflechissantLigne.cpp ObjetReflechissantBase.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Compilation de ObjetReflechissantArc.cpp
ObjetReflechissantArc.o: ObjetReflechissantArc.cpp ObjetReflechissantBase.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Compilation de Scene.cpp
Scene.o: Scene.cpp Scene.h ObjetReflechissantBase.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

# Nettoyage des fichiers objets
clean:
	rm *.o

####################

# Cible billard_sec532 : compilation des .cpp, édition des liens, puis exécution
billard_sec532: Util.o ObjetReflechissantBase.o ObjetReflechissantLigne.o main_sec532.o
	$(CPP) -o billard $^ $(LDFLAGS)
	./billard

# Compilation de main_sec532.cpp
main_sec532.o: main_sec532.cpp ObjetReflechissantBase.h ObjetReflechissantLigne.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

####################

billard_sec533: Util.o ObjetReflechissantBase.o ObjetReflechissantLigne.o ObjetReflechissantArc.o main_sec533.o
	$(CPP) -o billard $^ $(LDFLAGS)
	./billard

main_sec533.o: main_sec533.cpp ObjetReflechissantBase.h ObjetReflechissantLigne.h ObjetReflechissantArc.h Util.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

####################

billard_sec541: Util.o ObjetReflechissantBase.o ObjetReflechissantLigne.o ObjetReflechissantArc.o Scene.o main_sec541.o
	$(CPP) -o billard $^ $(LDFLAGS)
	./billard

main_sec541.o: main_sec541.cpp ObjetReflechissantBase.h ObjetReflechissantLigne.h ObjetReflechissantArc.h Util.h Scene.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

####################

billard_sec542: Util.o ObjetReflechissantBase.o ObjetReflechissantLigne.o ObjetReflechissantArc.o Scene.o main_sec542.o
	$(CPP) -o billard $^ $(LDFLAGS)
	./billard

main_sec542.o: main_sec542.cpp ObjetReflechissantBase.h ObjetReflechissantLigne.h ObjetReflechissantArc.h Util.h Scene.h PhaseSpaceDistrib.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)

####################

billard_sec543: Util.o ObjetReflechissantBase.o ObjetReflechissantLigne.o ObjetReflechissantArc.o Scene.o main_sec543.o
	$(CPP) -o billard $^ $(LDFLAGS)
	./billard

main_sec543.o: main_sec543.cpp ObjetReflechissantBase.h ObjetReflechissantLigne.h ObjetReflechissantArc.h Util.h Scene.h PhaseSpaceDistrib.h
	$(CPP) -o $@ -c $< $(CPPFLAGS)
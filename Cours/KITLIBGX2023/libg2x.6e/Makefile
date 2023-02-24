#==================================================
# version 6e (2022/2023) -
#==================================================
DEBUG = 0
CPP   = 0
VERS  = .6e
PROC  = .$(PROCBIT)

ifeq ($(CPP),1)
	# clang++ n'accepte pas les réels 1.0 sous type <float> => IL VEUT 1.0f => génère une erreur
	# g++ n'aime pas non plus, mais se contente d'un warning
	# beaucoup de warning liés à la gestion des chaînes de caractères
	CC   = g++
	STD  = -std=c++11
	LANG = ++
else
	CC  = clang
	STD = -std=c17
	LANG =
endif

ifeq ($(DEBUG),1)
  CFLAGS  = -O2 -g -Wpointer-arith -Wall
  DBG = .dbg
else
  CFLAGS = -O2
  DBG =
endif

LFLAGS = -L/usr/lib -lm -lGL -lGLU -lglut
PFLAGS = -I/usr/include -I./include

EXT = $(VERS)$(PROC)$(LANG)$(DBG)

src =  src/

# liste des modules
modules = g2x_window.o g2x_tools.o g2x_pixmap.o g2x_capture.o g2x_colors.o g2x_draw.o g2x_transfo.o g2x_geom.o g2x_polygon.o g2x_geoalgo.o g2x_particule.o

all : g2x clean

# compilation generique d'un module %.o a partir de src/%.c
%.o : $(src)%.c
	@echo "module $@"
	@$(CC) $(STD) -fpic $(CFLAGS) $(PFLAGS) -c $< -o $@
	@echo "------------------------"

# comilation generique d'un executable - edition de liens
% : $(modules) %.o
	@echo "assemblage [$^]->$@"
	$(CC) $^ $(LFLAGS) -o $@$(EXT)
	@echo "------------------------"

# compilation de libg2x
g2x : $(modules)
	@echo "massemblage bibliotheque dynamique lib$@$(EXT).so"
	@$(CC) -fpic -shared $^ $(LFLAGS) -o lib$@$(EXT).so
	@echo "assemblage bibliotheque d'archive lib$@$(EXT).a"
	@ar r lib$@$(EXT).a $^
	@echo "------------------------                                "

.PHONY : clean cleanall ?

# informations sur les paramètres de compilation
? :
	@echo "---------informations de compilation----------"
	@echo "  processeur     : $(PROCBIT)"
	@echo "  compilateur    : $(CC)"
	@echo "  standard       : $(STD)"
	@echo "  CFLAGS         : $(CFLAGS)"
	@echo "  LFLAGS         : $(LFLAGS)"
	@echo "  PFLAGS         : $(PFLAGS)"

clean :
	rm -f *.o

cleanall :
	rm -f *.o libg2x*.*

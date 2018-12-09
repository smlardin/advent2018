CXX := g++
CC  := gcc

CPPFLAGS := -std=c++11				\
				-Wall						\
				-Werror					\
				-g							\
				${DEBUG}					\

RUNFLAGS := ${RUNFLAGS}

# default to input.txt
ifeq (${RUNFLAGS},)
   RUNFLAGS := input.txt
endif

all: day tags

run: all
	./day_run ${RUNFLAGS} | tee output.txt

day: day.cc
	$(CXX) $(CPPFLAGS) $(INCDIRS) day.cc -o day_run

tags: day.cc
	ctags *.[ch]*



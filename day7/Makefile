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
	./day_part2_run ${RUNFLAGS} 1 | tee output.txt
	./day_part2_run ${RUNFLAGS} 5 | tee output_part2.txt

day: day.cc day_part2.cc
	$(CXX) $(CPPFLAGS) $(INCDIRS) day.cc -o day_run
	$(CXX) $(CPPFLAGS) $(INCDIRS) day_part2.cc -o day_part2_run

tags: day.cc
	ctags *.[ch]*



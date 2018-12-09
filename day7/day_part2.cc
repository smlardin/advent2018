#include <fstream>
#include <sstream>

#include <cstring>

#include <set>
#include <map>
#include <deque>
#include <list>
#include <set>

#include <inttypes.h>

#define LINE_SIZE    (1024)

using namespace std;

class step;

typedef pair<char, step *> step_id_t;

#ifndef BASE_MINUTES
#define BASE_MINUTES 60
#endif

class step
{
public:
   char name;
   bool complete;

   bool started;
   int seconds_remaining;

   set< step_id_t > preds;

   step() : name('_'), complete(false), started(false), seconds_remaining(BASE_MINUTES) { }
   step( char name ) : name(name), complete(false), started(false), seconds_remaining(BASE_MINUTES + (name - 'A' + 1)) { }

   bool remove( char pred_id )
   {
      bool erased = false;

      for (auto pred = preds.begin(); pred != preds.end(); pred++)
      {
         if (pred->first == pred_id)
         {
            preds.erase(pred);
            erased = true;
            break;
         }
      }

      return erased;
   }
};


typedef map< char, step > step_map_t;

char *trim( char *in_string )
{
   int in_index = 0;
   int out_index = strlen(in_string) - 1;

   while ((in_string[in_index] != '\r') && 
          (in_string[in_index] != '\n') &&
          (in_string[in_index] != 0))
   {
      if ((in_string[in_index] != '\t') &&
          (in_string[in_index] != ' ') 
         )
         break;

      in_index++;
   }

   //printf("starting out_index=%0d\n", out_index );

   while ((
          (in_string[out_index] == '\t') || 
          (in_string[out_index] == ' ')
          ) &&
             (in_string[out_index] != '\0') &&
             (out_index > 0)
          )
   {
      //printf(" out_index=%0d\n", out_index );
      out_index--;
   }

   //printf(" in_index=%0d out_index=%0d len=%0d\n", in_index, out_index, out_index - in_index + 1);

   strncpy( in_string, &(in_string[in_index]), out_index - in_index + 1 );

   in_string[out_index - in_index + 1] = '\0';

   //printf(" in_string: '%s'\n", in_string);

   return in_string;
}

void complete( step_map_t &step_list, char step_id )
{
   for (auto step = step_list.begin(); step != step_list.end(); step++)
      step->second.remove( step_id );

   return;
}

int main(int argc, char *argv[])
{
   char line[LINE_SIZE];
   step_map_t steps;
   list <char> finish_list;

   int workers = 1;
   int second = 0;

   if (argc < 2)
   {
      printf("%s input_file\n", argv[0]);

      exit(-1);
   }

   ifstream input_file(argv[1]);

   if (argc == 3)
      workers = atoi( argv[2] );

   do
   {
      if (!input_file.bad())
         input_file.clear();
      input_file.seekg( 0 );

#ifdef DEBUG
      printf("pre: input_file.eof=%0d input_file.fail=%0d input_file.bad=%0d\n", input_file.eof(), input_file.fail(), input_file.bad());
#endif

      while (!input_file.fail() && 
             !input_file.eof() && 
             (input_file.getline( line, LINE_SIZE ).good())
             )
      {
         char pred = strstr( line, "Step " )[5];
         char cur = strstr( line, "step " )[5];

         if (steps.find(cur) == steps.end())
            steps[cur] = step( cur );
         if (steps.find(pred) == steps.end())
            steps[pred] = step( pred );

         steps[cur].preds.insert( step_id_t( pred, &(steps[pred]) ) );
      }

   } while (0);

   printf("steps size=%0ld\n", steps.size());

   do 
   {
      // assign workers
      for (auto step = steps.begin(); step != steps.end(); step++)
      {
         if (!workers)
            break;

         if ((!step->second.started) &&
            (step->second.preds.size() == 0))
         {
            workers--;

            step->second.started = true;

#ifdef DEBUG
            printf("starting %c %0d\n", step->first, step->second.seconds_remaining);
#endif
         }
      }

      // decrement work for running segments
      for (auto step = steps.begin(); step != steps.end(); step++)
      {
         char step_id = step->first;

         if (step->second.started)
         {
            step->second.seconds_remaining--;
            if (!step->second.seconds_remaining)
            {
               finish_list.push_back(step_id);
               workers++;

#ifdef DEBUG
               printf("finishing %c workers=%0d\n", step_id, workers);
#endif
            }
         }
      }

      second++;

      // complete steps
      for (auto finish_it = finish_list.begin(); finish_it != finish_list.end(); finish_it++)
      {
         steps.erase( *finish_it );
         complete( steps, *finish_it );

         printf("%0d completing %c\n", second, *finish_it );
      }
      finish_list.clear();

   } while (steps.size());

   printf("\n");

   return 0;
}

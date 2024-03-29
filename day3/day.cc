#include <fstream>
#include <sstream>

#include <cstring>

#include <set>
#include <map>
#include <deque>
#include <list>

#include <inttypes.h>

#define LINE_SIZE    (1024)

using namespace std;

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

bool compare_string( string &string_1, string &string_2 )
{
   int diff = 0;
   unsigned int pos = 0;

   if (string_1.length() == string_2.length())
   {
      for (unsigned int letter = 0; letter < string_1.length(); letter++)
      {
         if (string_1[letter] != string_2[letter])
         {
            pos = letter;
            diff++;
         }
      }
   }

   if (diff == 1)
   {
      for (unsigned int letter = 0; letter < string_1.length(); letter++)
      {
         if (pos != letter)
            printf("%c", string_1[letter]);
      }
      printf("\n");
      return 1;
   }

   return 0;
}

typedef int elf_id_t;

int main(int argc, char *argv[])
{
   char line[LINE_SIZE];
   map < int,  map < int, list < elf_id_t > > > cloth;
   map < elf_id_t, bool > overlaps;

   int x_max = 0;
   int y_max = 0;
   int squares = 0;
   int non_squares = 0;

   if (argc != 2)
   {
      printf("%s input_file\n", argv[0]);

      exit(-1);
   }

   ifstream input_file(argv[1]);

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
         char * elf = strtok( line, " " );
         int elf_id = atoi(&(elf[1]));

         /* char * at  = */ strtok( NULL, " " );
         char * comma = strtok( NULL, ":" );
         char * square = trim( strtok( NULL, " " ) );

         int locationx = atoi( strtok( comma, "," ) );
         int locationy = atoi( strtok( NULL, " " ) );

         int run = atoi( strtok( square, "x" ) );
         int rise = atoi( strtok( NULL, " " ) );

         y_max = max(y_max, (locationy + rise));
         x_max = max(x_max, (locationx + run));

         overlaps[elf_id] = false;

         for (int x = locationx; x < locationx + run; x++)
         {
            for (int y = locationy; y < locationy + rise; y++)
            {
               cloth[x][y].push_back(elf_id);
#ifdef DEBUG
               printf("x=%0d y=%0d using=%0ld\n", x, y, cloth[x][y].size());
#endif

               if (cloth[x][y].size() > 1)
               {
                  for (auto elf_list_id = cloth[x][y].begin(); elf_list_id != cloth[x][y].end(); elf_list_id++)
                     overlaps[*elf_list_id] = true;
               }
            }
         }

#ifdef DEBUG
         printf("elf=%s comma=%s square=%s x=%0d y=%0d run=%0d rise=%0d\n", elf, comma, square, locationx, locationy, run, rise);
#endif
      }

      for (int x = 0; x < x_max; x++)
      {
         for (int y = 0; y < y_max; y++)
         {
            auto row = cloth.find(x);

            if (row != cloth.end())
            {
               auto square = row->second.find(y);

               if ((square != row->second.end()) && (square->second.size() > 1))
                  squares += 1;
               else 
                  non_squares += 1;
            }
            else
               non_squares += 1;
         }
      }

      printf("squares=%0d non-squares=%0d\n", squares, non_squares);

      for (auto overlap_index = overlaps.begin(); overlap_index != overlaps.end(); overlap_index++)
      {
#ifdef DEBUG
         printf("elf=%0d overlap=%0d\n", overlap_index->first, overlap_index->second);
#endif

         if (overlap_index->second == false)
         {
            printf("no overlap for elf=%0d\n", overlap_index->first );
         }
      }

   } while (0);

   return 0;
}

#include <fstream>
#include <sstream>

#include <cstring>

#include <set>

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

int main(int argc, char *argv[])
{
   size_t count = 0;

   char line[LINE_SIZE];
   int cur_val = 0;
   set<int> freq_seen;

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

      printf("pre: input_file.eof=%0d input_file.fail=%0d input_file.bad=%0d\n", input_file.eof(), input_file.fail(), input_file.bad());

      while (!input_file.fail() && 
             !input_file.eof() && 
             (input_file.getline( line, LINE_SIZE ).good())
             )
      {
         int input_val;
         
         if (trim(line)[0] == 0)
            continue;

         input_val = atoi(line);

         printf("Current frequency %0d, change of %0d; resulting frequency %0d size=%0ld\n", cur_val, input_val, cur_val + input_val, freq_seen.size() );

         cur_val += input_val;
         count++;

         if (count > 156000)
            break;

         if (freq_seen.find(cur_val) != freq_seen.end())
         {
            printf("  current frequency seen twice\n");
            break;
         }
         freq_seen.insert(cur_val);
      }

      printf("post: input_file.eof=%0d input_file.fail=%0d input_file.bad=%0d\n", input_file.eof(), input_file.fail(), input_file.bad());

   } while (input_file.eof() && !input_file.bad());


   return 0;
}

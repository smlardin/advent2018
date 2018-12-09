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

typedef list< char > reduce_string_t;

bool reduce_it( reduce_string_t &in_list )
{
   auto iter = in_list.begin();
   auto iter_next = iter++;

   do
   {
      if ( (iter != in_list.end()) && 
            (iter_next != in_list.end()) &&
            (*iter != *iter_next)
            && 
            (
             (*iter == toupper(*iter_next)) ||
             (*iter == tolower(*iter_next)) 
            )
         )
      {
         in_list.erase( iter );
         in_list.erase( iter_next );

         return 1;
      }

   } while ((++iter != in_list.end() ) && (++iter_next != in_list.end()));

   return 0;
}

int main(int argc, char *argv[])
{
   char rd_char;
   reduce_string_t reduce_string;
   int polymers;
   int total_reacts;

   char letter = 'a';

   char good_letter = 'a';
   int good_count = 0xfffffff;

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

      total_reacts = 0;
      reduce_string.clear();

      while (!input_file.fail() && 
             !input_file.eof() && 
             ((rd_char = input_file.get()) != 0) &&
             (rd_char != '\n') &&
             (rd_char != '\r')
             )
      {
         total_reacts++;
         if (tolower(rd_char) != letter)
         {
            reduce_string.push_back(rd_char);
         }
      }

      printf("starting reduction without '%c' string size %0d (actual %0ld)\n", letter, total_reacts, reduce_string.size());

      while (reduce_it(reduce_string));

      polymers = reduce_string.size();

      printf("remove '%c' %0d polymers remain of %0d starting\n", letter, polymers, total_reacts );

      if (polymers < good_count)
      {
         good_count = min(good_count, polymers);
         good_letter = letter;
      }

   } while (letter++ < 'z');

   printf("min by removing '%c' of size %0d\n", good_letter, good_count );

   return 0;
}

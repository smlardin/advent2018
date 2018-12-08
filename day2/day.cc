#include <fstream>
#include <sstream>

#include <cstring>

#include <set>
#include <map>
#include <deque>

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

int main(int argc, char *argv[])
{
   char line[LINE_SIZE];
   int code_3 = 0, code_2 = 0;
   deque< string > all_strings;

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

//    printf("pre: input_file.eof=%0d input_file.fail=%0d input_file.bad=%0d\n", input_file.eof(), input_file.fail(), input_file.bad());

      while (!input_file.fail() && 
             !input_file.eof() && 
             (input_file.getline( line, LINE_SIZE ).good())
             )
      {
         map<char, int> letter_count;
         int num_2, num_3;

         num_2 = 0;
         num_3 = 0;

         if (trim(line)[0] == 0)
            continue;

         all_strings.push_back(line);

         for (unsigned int letter = 0; letter < strlen(line); letter++)
         {
            letter_count[ line[letter] ]++;
            if (letter_count[ line[ letter ] ] == 3)
               num_3 ++, num_2 --;
            if (letter_count[ line[ letter ] ] == 2)
               num_2 ++;
            if (letter_count[ line[ letter ] ] == 4)
               num_3 --;
         }

         letter_count.clear();

         if (num_3)
            code_3++;
         if (num_2)
            code_2++;

         printf("%s: num_2=%0d num_3=%0d code_2=%0d code_3=%0d\n", line, num_2, num_3, code_2 , code_3 );
      }
   } while (0);

   // iterate all of the string to find ones which may or may not match
   for (unsigned int high = 0; high < all_strings.size() - 1; high++)
   {
      for (unsigned int low = high + 1; low < all_strings.size(); low++)
      {
         if (compare_string( all_strings[low], all_strings[high] ))
            printf("matched %s and %s\n", all_strings[low].c_str(), all_strings[high].c_str() );
      }
   }

   printf("checksum = %0d * %0d = %0d\n", code_2, code_3, code_2 * code_3 );

   return 0;
}

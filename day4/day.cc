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

struct month_days_t 
{
   int month;
   int days;
}
month_days[] = 
{
   { 1,  31   },  // Jan
   { 2,  28   },  // Feb
   { 3,  31   },  // Mar
   { 4,  30   },  // Apr
   { 5,  31   },  // May
   { 6,  30   },  // June
   { 7,  31   },  // July
   { 8,  31   },  // August
   { 9,  30   },  // Sept
   { 10, 31   },  // Oct
   { 11, 30   },  // Nov
   { 12, 31   },  // Dec
};  

class shift
{
   public:
      typedef enum state_t
      {
         UNDEFINED = 0,
         ASLEEP = 1,
         AWAKE = 2,
      } state_t;

      int     guard_id;
      state_t minutes[60];
      int     minutes_asleep;
      bool    fixed;

      shift() : guard_id(0), minutes_asleep(0), fixed(false) {
         memset( minutes, 0, sizeof(minutes) );
      }

      void fix_timeline()
      {
         state_t state = AWAKE;

         if (fixed)
            return;
         fixed = true;

         for (int minute = 0; minute < 60; minute++)
         {
            if (minutes[minute] == UNDEFINED)
               minutes[minute] = state;
            else 
               state = minutes[minute];

            if (state == ASLEEP)
               minutes_asleep++;
         }
      }

      state_t get_minute_state(int minute)
      {
         fix_timeline();

         return minutes[minute];
      }

      int get_minutes_asleep()
      {
         fix_timeline();

         return minutes_asleep;
      }

      void print_shift()
      {
         printf("  ");
         for (unsigned int minute = 0; minute < sizeof(minutes)/sizeof(state_t); minute++)
            printf("%c", (minutes[minute] == AWAKE) ? '.' : '#' );

         printf("\n");

         return;
      }
};

class guard
{
   public:
      int common_minute;
      int common_count;

      list < shift * > shifts;

      guard() : common_minute(0), common_count(0) { }

      void add_shift( shift * in_shift )
      {
         shifts.push_back(in_shift);
      }

      int get_minutes_asleep()
      {
         int minutes = 0;

         for (auto shift = shifts.begin(); shift != shifts.end(); shift++)
            minutes += (*shift)->get_minutes_asleep();

         return minutes;
      }

      int get_common_minute_count()
      {
         get_common_minute();

         return common_count;
      }

      int get_common_minute()
      {
         int common_max = 0;

         if (common_count == 0)
         {
            for (int minute = 0; minute < 60; minute++ )
            {
               int asleep = 0;

               for (auto shift = shifts.begin(); shift != shifts.end(); shift++)
               {
                  if ((*shift)->get_minute_state(minute) == shift::ASLEEP)
                     asleep++;
               }

               if (asleep > common_max)
               {
                  common_max = asleep;
                  common_minute = minute;
               }
            }

            common_count = common_max;
         }


         return common_minute;
      }

      void print_shifts()
      {
         for (auto shift = shifts.begin(); shift != shifts.end(); shift++)
         {
            (*shift)->print_shift();
         }
      }
};

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
   map < string, shift > elf_shift;
   map < int, guard > guard_shifts;

   struct {
      int guard_id;
      int count;
      int minute;
   } answer_info[2];

   memset( &answer_info[0], 0, sizeof(answer_info) );
   memset( &answer_info[1], 0, sizeof(answer_info) );

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
         char *guard_id_str;
         if ((guard_id_str = strstr(line, "#")) != NULL)
         {
            int guard_id = atoi(&(guard_id_str[1]));

            char *guard_date = &(strstr(line, "-")[1]);
            int guard_time = atoi(&(strstr(line, " ")[1]));
            int month = atoi(guard_date);

            guard_date[5] = '\0';

            // fix the guard date if it begins the previous day (argh!?!)
            if (guard_time != 0)
            {
               int day = atoi(&guard_date[3]);

               day = day + 1;
               if (day > month_days[month-1].days)
               {
                  month += 1;
                  day = 1;
               }
               sprintf( guard_date, "%02d-%02d", month, day );
#ifdef DEBUG
               printf("updating guard date %s\n", guard_date);
#endif
            }

            elf_shift[guard_date].guard_id = guard_id;
            guard_shifts[guard_id].add_shift( &(elf_shift[guard_date]) );

#ifdef DEBUG
            printf("updating guard shift date=%s guard_id=%d\n", guard_date, guard_id);
#endif
         }
         else 
         {
            char *shift_date = &(strstr(line, "-")[1]);
            bool asleep = (strstr(line, "falls asleep") != NULL);
            int minute = atoi( &(strstr( line, ":" )[1]) );

            shift_date[5] = '\0';
#ifdef DEBUG
            printf("updating shift date=%s\n", shift_date);
#endif

            elf_shift[shift_date].minutes[ minute ] = asleep ? shift::ASLEEP : shift::AWAKE ;
         }
      }

   } while (0);

   for (auto guard = guard_shifts.begin(); guard != guard_shifts.end(); guard++ )
   {

#ifdef DEBUG
      printf("guard_id=%0d minutes_asleep=%0d common_minute=%0d common_count=%0d\n", 
            guard->first, 
            guard->second.get_minutes_asleep(), 
            guard->second.get_common_minute(),
            guard->second.get_common_minute_count()
            );
      guard->second.print_shifts();
#endif

      if (guard->second.get_minutes_asleep() > answer_info[0].count)
      {
         answer_info[0].count = guard->second.get_minutes_asleep();
         answer_info[0].guard_id = guard->first;
         answer_info[0].minute = guard->second.get_common_minute();
      }

      printf("guard_id=%0d gcmc=%0d ac=%0d\n", guard->first, guard->second.get_common_minute_count(), answer_info[1].count );
      printf("guard_id=%0d gcmc=%0d ac=%0d\n", guard->first, guard->second.get_common_minute_count(), answer_info[1].count );

      if (guard->second.get_common_minute_count() > answer_info[1].count)
      {
         answer_info[1].count = guard->second.get_common_minute_count();
         answer_info[1].guard_id = guard->first;
         answer_info[1].minute = guard->second.get_common_minute();
      }
   }

   for (int strat = 0; strat < 2; strat ++ )
   {
      printf("guard strategy %0d id=%0d minute=%0d answer=%0d\n", strat + 1, 
            answer_info[strat].guard_id, 
            answer_info[strat].minute, 
            answer_info[strat].guard_id * answer_info[strat].minute 
            );
   }

   return 0;
}

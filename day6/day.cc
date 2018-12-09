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

typedef pair< int, int > distance_id_t;
typedef set< distance_id_t > distance_set_t;

class point 
{

public:
   int id;
   int x;
   int y;

   point() : id(0), x(0), y(0) {}
   point(int id, int x, int y) : id(id), x(x), y(y) {}

};

typedef list< point > points_t;

class grid_entry
{
public:
   distance_set_t distances;

   bool is_uniq;
   int uniq_id;

   int total_distance;

   int x;
   int y;

   grid_entry( int x, int y ) : is_uniq(true), uniq_id(-1), total_distance(0), x(x), y(y) { }

   void calculate_distances( points_t &all_points )
   {
      for (auto point = all_points.begin(); point != all_points.end(); point++)
      {
         int distance = abs( point->x - x ) + abs( point->y - y );

         total_distance += distance;

         distances.insert( distance_id_t( distance, point->id ) );
      }

      if (distances.begin()->first == (++distances.begin())->first)
         is_uniq = false;

      if (is_uniq)
         uniq_id = distances.begin()->second;

#ifdef DEBUG_DL
      printf("distance list size=%0ld front_distance=%0d uniq=%0d total_distance=%0d\n", 
            distances.size(), 
            distances.begin()->first, 
            is_uniq,
            total_distance);
#endif
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
   points_t all_points;
   int max_x = 0, max_y = 0;

   map< int, list < grid_entry > > grid_list;
   set< distance_id_t > area_list;

   list< grid_entry > safe_region;

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
         int x = atoi( trim( strtok( line, "," ) ) );
         int y = atoi( trim( strtok( NULL, " " ) ) );

         all_points.push_back( point( all_points.size(), x, y ) );

         max_x = max(x, max_x);
         max_y = max(y, max_y);

#ifdef DEBUG
         printf("add point x=%0d y=%0d id=%0ld total=%0ld\n", x, y, all_points.size() - 1, all_points.size());
#endif
      }

   } while (0);

#ifndef DISTANCE_COND
#define DISTANCE_COND 10000
#endif

#ifndef BOUND_COND
#define BOUND_COND   2
#endif
   for (int x_index = -BOUND_COND; x_index < max_x + BOUND_COND; x_index++)
   {
      for (int y_index = -BOUND_COND; y_index < max_y + BOUND_COND; y_index++)
      {
         grid_entry square(x_index, y_index);

         square.calculate_distances( all_points );

         if (square.is_uniq)
         {
            grid_list[ square.uniq_id ].push_back(square);
#ifdef DEBUG_UNIQ
            printf("uniq square x=%0d y=%0d id=%0d count=%0ld\n", x_index, y_index, square.uniq_id, grid_list[ square.uniq_id ].size() );
#endif
         }

         if (square.total_distance < DISTANCE_COND)
         {
            if ((x_index < 0 || x_index > max_x) ||
               (y_index < 0 || y_index > max_y))
            {
               printf("error safe_region close to all points x=%0d y=%0d\n", x_index, y_index);
            }
            safe_region.push_back(square);
         }
      }
   }

   for (auto area = grid_list.begin(); area != grid_list.end(); area++)
   {
      bool infinity_item = false;

      for (auto grid_item = area->second.begin(); grid_item != area->second.end(); grid_item++)
      {
         if ((grid_item->x < 0) || (grid_item->x > max_x))
            infinity_item = true;
         if ((grid_item->y < 0) || (grid_item->y > max_y))
            infinity_item = true;
      }

      if (!infinity_item)
         area_list.insert( distance_id_t( area->second.size(), area->first ) );
   }

   for (auto area = area_list.begin(); area != area_list.end(); area++)
   {
      printf("size=%0d area_id=%0d\n", area->first, area->second );
   }

   printf("safe region size=%0ld\n", safe_region.size());

   return 0;
}

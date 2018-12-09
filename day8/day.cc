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

typedef list< int > int_stream_t;
int_stream_t int_stream;

class node 
{
public:
   static int g_node_id;

   int node_id;

   unsigned int child_node_count;
   unsigned int meta_data_count;
   unsigned int meta_sum;

   deque<node> child_nodes;
   deque<unsigned int>  meta_data;

   node() : node_id(0), child_node_count(0), meta_data_count(0), meta_sum(0) {}

   void create( int_stream_t &stream )
   {
      node_id = g_node_id++;

      child_node_count = stream.front();
      stream.pop_front();

      meta_data_count = stream.front();
      stream.pop_front();

      printf("node id=%0d starting\n", node_id );

      for (unsigned int index = 0; index < child_node_count; index++)
      {
         node new_node;

         new_node.create( stream );

         child_nodes.push_back( new_node );
      }

      for (unsigned int index = 0; index < meta_data_count; index++)
      {
         meta_data.push_back( stream.front() );
         meta_sum += meta_data.back();
         stream.pop_front();
      }

      printf("node id=%0d contains %0d children(%0ld) %0d meta(%0ld) sum=%0d\n",
            node_id,
            child_node_count,
            child_nodes.size(),
            meta_data_count,
            meta_data.size(),
            meta_sum);

      return;
   }

   int get_node_value()
   {
      int sum = 0;

      if (child_nodes.size() == 0)
      {
         return meta_sum;
      }

      for (auto &data: meta_data )
      {
         if ((data == 0) || (data > child_nodes.size()))
            continue;

         sum += child_nodes[data-1].get_node_value();
      }

      return sum;
   }

   int get_meta_count()
   {
      int sum = 0;

      for (auto &data: meta_data )
         sum += data;

      for (auto child = child_nodes.begin(); child != child_nodes.end(); child++)
         sum += child->get_meta_count();

      return sum;
   }

};

int node::g_node_id = 0;

int main(int argc, char *argv[])
{
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
             !input_file.eof())
      {
         int value;

         input_file >> value;

         if (!input_file.eof())
            int_stream.push_back( value );
      }

   } while (0);

   // create the node tree
   node node_tree;

   node_tree.create( int_stream );

   printf("meta count=%0d\n", node_tree.get_meta_count() );
   printf("node value=%0d\n", node_tree.get_node_value() );

   return 0;
}

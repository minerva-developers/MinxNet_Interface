#include <iostream>
using namespace std;

#include "dagengine.h"
#include "operations.h"

extern DAGEngine * engine = new SingleThreadDAGEngine;

int main()
{
  /*DAGEngine * engine = new SingleThreadDAGEngine;
  for (int i = 0; i < 10; i++)
  {
    NodeId nodeId = engine->PushNode([i](){cout << "executing " << i << endl; }, std::vector<NodeId>{ NodeId(i-1) });
  }*/

  NArray a, b;
  a.scale = { 3, 2 };
  b.scale = { 3, 2 };

  NArray c = Add(a, b);


  return 0;
}
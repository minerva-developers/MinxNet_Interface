#include <iostream>
using namespace std;

#include "dagengine.h"
#include "all_ops.h"

DAGEngine * engine = new SingleThreadDAGEngine;
StorageManager * storage = new NaiveStorageManager;

int main()
{
  const static int M = 1024;
  const static int N = 512;
  const static int K = 1000;
  FloatT delta = 0.24;
  FloatT * customA = new FloatT[M * N];
  DummyLayer layer(delta);
  NArray layerIn({M, N}, customA);
  NArray layerOut({ M, N });
  FF({ layerIn }, { &layerOut }, &layer);
  NArray x = layerIn + layerOut;
  WaitForAll();

  NArray a({ 1024, 768 });
  NArray b({ 1024, 768 });
  NArray c({ 1024, 768 });
  NArray d = a + b + c;

  return 0;
}


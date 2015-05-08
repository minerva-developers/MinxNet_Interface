#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include "common.h"
#include "dagengine.h"

DAGEngine * engine;

typedef std::vector<int> Scale;

struct NArray {
  // dummy class for tracking dependencies
  bool owns_data = false;
  FloatT * data = nullptr;
  NodeId dagNode;
  Scale scale;
};

template<int NI, int NO>
class Operation {
public:
  const int nInputs = NI;
  const int nOutputs = NO;
  Operation(NArray inputs[NI], NArray outputs[NO]) {
    for (int i = 0; i < NI; i++)
    {
      input[i] = inputs[i];
    }
    for (int i = 0; i < NO; i++)
    {
      output[i] = outputs[i];
    }
  }
  virtual void execute() = 0;
  virtual void validate() {}

  void composeDag() {
    NodeId predNodes[NI];
    for (int i = 0; i < NI; i++) {
      predNodes[i] = input[i].dagNode;
    }
    NodeId nodeId = engine->PushNode([this](){this->execute(); }, 
      std::vector<NodeId>(&predNodes[0], &predNodes[0] + NI));
    for (int i = 0; i < NO; i++) {
      output[i].dagNode = nodeId;
    }
  }
protected:
  NArray input[NI];
  NArray output[NO];
};

class Addition : public Operation<2, 1> {
public:
  Addition(NArray inputs[2], NArray outputs[1]) : Operation<2,1>(inputs, outputs) {
  }

  void validate() override{
    assert(input[0].scale == input[1].scale);
    assert(input[0].scale == output[0].scale);
  }

  void execute() override {
    // performing addition
    std::cout << "doing addition of matrix " << input[0].scale.size() << "{";
    const Scale & scale = input[0].scale;
    for (unsigned int i = 0; i < scale.size(); i++) {
      if (i != 0)
        std::cout << ",";
      std::cout << scale[i];
    }
    std::cout << "}" << std::endl;
  }
};

template<class OP>
void DoOperation(NArray inputs[], NArray outputs[]) {
  OP op(inputs, outputs);
  op.composeDag();
}

NArray Add(const NArray & a, const NArray & b) {
  NArray c;
  c.scale = a.scale;
  NArray ins[2] = { a, b };
  NArray outs[1] = { c };
  DoOperation<Addition>(ins, outs);
  return c;
}
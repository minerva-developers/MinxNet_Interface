#pragma once

#include <memory>
#include <cstdint>
#include "storage.h"
#include "common.h"
#include "dagengine.h"

extern StorageManager * storage;
extern DAGEngine * engine;

struct NArray {
  NArray(const Scale & scale_) : scale(scale_) {
    sid = storage->makeNewInstance(Capacity(scale));
  }
  NArray(const Scale & scale_, void * memory) : scale(scale_) {
    // TODO: use correct device
    sid = storage->makeNewInstance(memory, Capacity(scale), 0);
  }
  Scale scale;
  NodeId dagNode = 0;
  StorageId sid;
};

struct Blob {
  FloatT * data;
  Scale scale;
};

NArray GuassianRand(Scale s, double avg, double vari);
NArray SoftMax(const NArray & arr);
NArray operator+ (const NArray & a, const NArray & b);
NArray operator* (const NArray & a, const NArray & b);

struct StorageWithShape {
  Scale scale;
  StorageId sid;
};

template<class OP>
void ScheduleOP(const OP & op,
  const std::vector<NArray> & inputs,
  const std::vector<NArray*> & outputs)
{
  // get storage id for use when executing
  std::vector<StorageWithShape> inputStorage(inputs.size());
  std::vector<StorageWithShape> outputStorage(outputs.size());
  for (size_t i = 0; i < inputs.size(); i++) {
    inputStorage[i] = { inputs[i].scale, inputs[i].sid };
    // add reference to avoid GC
    storage->ref(inputStorage[i].sid);
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    outputStorage[i] = { outputs[i]->scale, outputs[i]->sid };
    // add reference to avoid GC
    storage->ref(outputStorage[i].sid);
  }
  // compute dependency
  std::vector<NodeId> predNodes(inputs.size());
  for (size_t i = 0; i < inputs.size(); i++) {
    predNodes[i] = inputs[i].dagNode;
  }
  NodeId nodeId = engine->PushNode([op, inputStorage, outputStorage](){
    // TODO: get real device id
    DeviceId device = 0;
    // prepare input/output
    std::vector<Blob> inputs(inputStorage.size());
    std::vector<Blob> outputs(outputStorage.size());
    for (size_t i = 0; i < inputs.size(); i++) {
      inputs[i].data = (FloatT*)storage->getStorage(inputStorage[i].sid, device);
      inputs[i].scale = inputStorage[i].scale;
    }
    for (size_t i = 0; i < outputs.size(); i++) {
      outputs[i].data = (FloatT*)storage->getStorage(outputStorage[i].sid, device);
      outputs[i].scale = outputStorage[i].scale;
    }
    // TODO: run on different devices
    bool isOnCPU = true;
    if (isOnCPU) {
      op.executeCPU(inputs, outputs);
    }
    else {
      op.executeGPU(inputs, outputs);
    }
  },
    predNodes);
  for (size_t i = 0; i < outputs.size(); i++) {
    outputs[i]->dagNode = nodeId;
  }
}

class NArrayOperator {
public:
  virtual void executeCPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const = 0;   // execute on cpu
  virtual void executeGPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const = 0;  // execute on gpu
};


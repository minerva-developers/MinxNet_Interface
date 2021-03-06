#pragma once
#include "narray.h"

class Layer {
public:
  virtual void forward_cpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) = 0;
  virtual void forward_gpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) = 0;
  virtual void backward_cpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs,
    const std::vector<bool> & propagate_down = {}) = 0;
  virtual void backward_gpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs,
    const std::vector<bool> & propagate_down = {}) = 0;
};

class FFOperator : public NArrayOperator {
public:
  FFOperator(Layer * layer_) : layer(layer_) {}
  virtual void executeCPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const {
    layer->forward_cpu(inputs, outputs);
  }
  virtual void executeGPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const {
    layer->forward_gpu(inputs, outputs);
  }
private:
  Layer * layer;
};

class BPOperator : public NArrayOperator {
public:
  BPOperator(Layer * layer_) : layer(layer_) {}
  virtual void executeCPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const {
    layer->backward_cpu(inputs, outputs);
  }
  virtual void executeGPU(const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) const {
    layer->backward_gpu(inputs, outputs);
  }
private:
  Layer * layer;
};

inline void FF(const std::vector<NArray> & inputs,
  const std::vector<NArray*> & outputs, Layer * layer) {
  ScheduleOP(FFOperator(layer), inputs, outputs);
}

inline void BP(const std::vector<NArray> & inputs,
  const std::vector<NArray*> & outputs, Layer * layer) {
  ScheduleOP(BPOperator(layer), inputs, outputs);
}
#pragma once

#include <iostream>
#include "layer.h"

class DummyLayer : public Layer {
  // dummy layer that adds delta to matrix when ff, and substracts when bp
public:
  DummyLayer(FloatT delta_) : delta(delta_) {}
  virtual void forward_cpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) {
    std::cout << "ff cpu" << std::endl;
  }
  virtual void forward_gpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs) {
    std::cout << "ff gpu" << std::endl;
  }
  virtual void backward_cpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs,
    const std::vector<bool> & propagate_down = {}) {
    std::cout << "bp cpu" << std::endl;
  }
  virtual void backward_gpu(
    const std::vector<Blob> & inputs,
    const std::vector<Blob> & outputs,
    const std::vector<bool> & propagate_down = {}) {
    std::cout << "bp gpu" << std::endl;
  }
private:
  FloatT delta;
};
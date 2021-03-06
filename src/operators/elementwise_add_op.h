/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifdef ELEMENTWISEADD_OP

#pragma once

#include <string>
#include "framework/operator.h"
#include "kernel/elementwise_add_kernel.h"
#include "operators/op_param.h"

namespace paddle_mobile {
namespace operators {
using std::string;
template <typename DeviceType, typename T>
class ElementwiseAddOp : public framework::OperatorWithKernel<DeviceType> {
 public:
  ElementwiseAddOp(const string &type, const VariableNameMap &inputs,
                   const VariableNameMap &outputs,
                   const framework::AttributeMap attrs,
                   std::shared_ptr<framework::Scope> scope)
      : framework::OperatorWithKernel<DeviceType>(type, inputs, outputs, attrs,
                                                  scope),
        param_(inputs, outputs, attrs, *scope) {}

  void RunImpl() const {
    operators::ElementwiseAddKernel<DeviceType, T> kernel;
    kernel.Compute(param_);
  }

  using framework::OperatorWithKernel<DeviceType>::OperatorWithKernel;
  void InferShape() const override;

 protected:
  ElementwiseAddParam param_;
};
}  // namespace operators
}  // namespace paddle_mobile

#endif

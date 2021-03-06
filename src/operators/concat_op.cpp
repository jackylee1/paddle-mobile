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

#ifdef CONCAT_OP

#include "concat_op.h"

namespace paddle_mobile {
namespace operators {

template <typename Dtype, typename T>
void ConcatOp<Dtype, T>::InferShape() const {
  auto inputs = param_.Inputs();
  const size_t n = inputs.size();

  std::vector<DDim> inputs_dims;
  inputs_dims.reserve(n);
  for (int i = 0; i < n; i++) {
    inputs_dims.push_back(inputs[i]->dims());
  }

  auto axis = static_cast<size_t>(param_.Axis());

  if (n == 1) {
    DLOG << "Warning: concat op have only one input, "
            "may waste memory";
  }

  /// add all dim[axis] and check other dims if equal.
  auto out_dims = inputs_dims[0];
  int in_zero_dims_size = out_dims.size();
  for (size_t i = 1; i < n; i++) {
    for (size_t j = 0; j < in_zero_dims_size; j++) {
      if (j == axis) {
        out_dims[axis] += inputs_dims[i][j];
      } else {
        assert(out_dims[j] == inputs_dims[i][j]);
      }
    }
  }

  if (out_dims[axis] < 0) {
    out_dims[axis] = -1;
  }

  param_.Out()->Resize(out_dims);
}
template class ConcatOp<CPU, float>;

}  // namespace operators
}  // namespace paddle_mobile

namespace ops = paddle_mobile::operators;
USE_OP(concat);
REGISTER_OPERATOR(concat, ops::ConcatOp);

#endif

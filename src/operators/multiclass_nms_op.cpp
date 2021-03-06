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

#ifdef MULTICLASSNMS_OP

#include "operators/multiclass_nms_op.h"
namespace paddle_mobile {
namespace operators {

template <typename Dtype, typename T>
void MultiClassNMSOp<Dtype, T>::InferShape() const {
  auto input_bboxes_dims = param_.InputBBoxes()->dims();
  auto input_scores_dims = param_.InputScores()->dims();
  if (input_scores_dims.size() != 3) {
    LOG(kLOG_ERROR) << "Input Scores size must be 3";
  }
  if (input_bboxes_dims[2] != 4) {
    LOG(kLOG_ERROR) << "Input BBoxes 2nd dimension must be 4";
  }
  if (input_bboxes_dims[1] != input_scores_dims[2]) {
    LOG(kLOG_ERROR) << "Predict bboxes must be equal";
  }
  // pre size, will change in Compute.
  param_.Out()->Resize(framework::make_ddim({input_bboxes_dims[1], 6}));
}
template class MultiClassNMSOp<CPU, float>;
}  // namespace operators
}  // namespace paddle_mobile

namespace ops = paddle_mobile::operators;
USE_OP(multiclass_nms);
REGISTER_OPERATOR(multiclass_nms, ops::MultiClassNMSOp);

#endif

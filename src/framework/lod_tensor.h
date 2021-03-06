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

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "tensor.h"
#include "tensor_util.h"

namespace paddle_mobile {

namespace framework {

/*
 * LoD is short for Level of Details.
 *
 * - in a level, each element indicates relative offset of the lower
 * level
 * - the first element should be 0 and that indicates that this sequence
 * start
 * from 0
 * - each sequence's begin and end(no-inclusive) is level[id, id+1]
 *
 * For example:
 *    3-level LoD stores
 *
 *    0 2 3
 *    0 2 4 7
 *    0 2 5 7 10 12 15 20
 */
using LoD = std::vector<std::vector<size_t>>;

std::ostream &operator<<(std::ostream &os, const LoD &lod);

std::ostream &operator<<(std::ostream &os, const LoDTensor &t);

std::string LoDToString(const LoD &lod);

LoD SliceInLevel(const LoD &in, size_t level, size_t elem_begin,
                 size_t elem_end);

/*
 * Transform an LoD from relative offsets to absolute offsets.
 */
LoD ToAbsOffset(const LoD &in);

bool operator==(const LoD &a, const LoD &b);

/*
 * Check whether this lod's format is valid.
 *
 * ATTENTION:
 *   - Empty lod is treated as valid.
 *
 * It will check two things:
 *
 *  1. all the offsets in a level should be ascending(no same items
 * allows).
 *  2. there should be more than 2 offsets existing in each level.
 *  3. the higher level's last offset should equals the lower level's
 * size-1.
 *  4. the first offset(the begin offset) of each level should be 0.
 *  5. the lowest level's last offset should equals `tensor_height` if
 * tensor_height>0.
 */

bool CheckLoD(const LoD &in, int tensor_height = -1);

/*
 * Check whether this absolute lod's format is valid.
 *
 * ATTENTION:
 *   - Empty lod is treated as valid.
 *
 * It will check two things:
 *  1. all the offsets in a level should be ascending(no same items
 * allows)
 *  2. there should be more than 2 offsets existing in each level.
 *  3. the first offset of each level should be 0, and the last should
 * be the
 *     same(the height of underlying tensor) or `tensor_height` if
 *     tensor_height>0.
 */
bool CheckAbsLoD(const LoD &in, int tensor_height = -1);

/*
 * LoDTensor (Level of details Tensor)
 * see https://en.wikipedia.org/wiki/Level_of_details for reference.
 */
class LoDTensor : public Tensor {
 public:
  LoDTensor() : Tensor() {}

  explicit LoDTensor(const LoD &lod) : lod_(lod) {}

  void set_lod(const LoD &lod) { lod_ = lod; }

  const LoD &lod() const { return lod_; }

  LoD *mutable_lod() { return &lod_; }

  /*
   * Get the start offset and end offset of an  element from LoD.
   */
  std::pair<size_t, size_t> lod_element(size_t level, size_t elem) const {
    //    PADDLE_ENFORCE_LT(level, NumLevels());
    //    PADDLE_ENFORCE_LT(elem, NumElements(level));
    return std::make_pair((lod_)[level][elem], (lod_)[level][elem + 1]);
  }

  /*
   * Number of LoDTensor's levels, each level has units of data, for
   * example,
   * in the sentence's view, article, paragraph, sentence are 3
   * levels.
   */
  size_t NumLevels() const { return lod_.size(); }

  /*
   * Number of elements in a level.
   */
  size_t NumElements(size_t level = 0) const {
    //    PADDLE_ENFORCE_LT(level, NumLevels());
    // the last offset is the end of last element
    return (lod_)[level].size() - 1;
  }

 private:
  LoD lod_;
};

/*
 * Expand the `source` to fit the LoD of `lod`. For example, a `source`
 * LoDTensor is
 *  - LoD: [0, 2]
 *  - tensor: [a0, a1]
 * a `lod` is
 *  - LoD: [0 3 5]
 * returns a new LoDTensor
 *  - [a0 a0 a0 a1 a1]
 */
template <typename T>
LoDTensor LodExpand(const LoDTensor &source, const LoD &lod, size_t level) {
  LoD abs_lod = ToAbsOffset(lod);
  const auto &lod_level = lod[level];
  size_t num_instances = source.dims()[0];

  // new tensor
  LoDTensor tensor;
  tensor.set_lod(lod);
  auto dims = source.dims();
  dims[0] = lod_level.back();
  tensor.Resize(dims);
  tensor.mutable_data<T>();

  //  PADDLE_ENFORCE_EQ(num_instances, lod_level.size() - 1);
  for (size_t ins = 0; ins < num_instances; ins++) {
    for (size_t elem = lod_level[ins]; elem < lod_level[ins + 1]; elem++) {
      auto slice = tensor.Slice(elem, elem + 1);
      TensorCopy(source.Slice(ins, ins + 1), &slice);
    }
  }
  return tensor;
}

// Get the absolute offset of a lod[start_level][start_idx:end_idx] and
// relative length of details for every levels(i.e., [start_level: ]).
//
// For example,
//   lod = [[0, 3, 4, 8], [0, 9, 10, 11, 13, 17, 19, 22, 24]]
//   start_level = 0
//   start_idx = 1
//   end_idx = 3
//
// Returns:
//  LoD = [[1, 4], [2, 4, 2, 3, 2]]
//  pair<size_t, size_t> = {11, 24}
std::pair<LoD, std::pair<size_t, size_t>> GetSubLoDAndAbsoluteOffset(
    const LoD &lod, size_t start_idx, size_t end_idx, size_t start_level);

void AppendLoD(LoD *lod, const LoD &lod_length);

/*
 * Serialize/Desiralize LoDTensor to std::ostream
 * You can pass ofstream or ostringstream to serilize to file
 * or to a in memory string. GPU tensor will be copied to CPU.
 */
void SerializeToStream(std::ostream &os, const LoDTensor &tensor);

void DeserializeFromStream(std::istream &is, LoDTensor *tensor);

}  // namespace framework
}  // namespace paddle_mobile

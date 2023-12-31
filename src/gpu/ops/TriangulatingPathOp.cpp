/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2023 THL A29 Limited, a Tencent company. All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "TriangulatingPathOp.h"
#include "core/PathRef.h"
#include "gpu/Gpu.h"
#include "gpu/processors/DefaultGeometryProcessor.h"

namespace tgfx {
std::unique_ptr<TriangulatingPathOp> TriangulatingPathOp::Make(Color color, const Path& path,
                                                               const Rect& clipBounds,
                                                               const Matrix& localMatrix) {
  if (path.countVerbs() > AA_TESSELLATOR_MAX_VERB_COUNT) {
    return nullptr;
  }
  std::vector<float> vertices = {};
  int count = PathRef::ToAATriangles(path, clipBounds, &vertices);
  if (count == 0) {
    return nullptr;
  }
  return std::make_unique<TriangulatingPathOp>(color, vertices, count, path.getBounds(),
                                               Matrix::I(), localMatrix);
}

TriangulatingPathOp::TriangulatingPathOp(Color color, std::shared_ptr<GpuBuffer> buffer,
                                         int vertexCount, const Rect& bounds,
                                         const Matrix& viewMatrix, const Matrix& localMatrix)
    : DrawOp(ClassID()), color(color), buffer(std::move(buffer)), vertexCount(vertexCount),
      viewMatrix(viewMatrix), localMatrix(localMatrix) {
  setBounds(bounds);
}

TriangulatingPathOp::TriangulatingPathOp(Color color, std::vector<float> vertices, int vertexCount,
                                         const Rect& bounds, const Matrix& viewMatrix,
                                         const Matrix& localMatrix)
    : DrawOp(ClassID()), color(color), vertices(std::move(vertices)), vertexCount(vertexCount),
      viewMatrix(viewMatrix), localMatrix(localMatrix) {
  setBounds(bounds);
}

bool TriangulatingPathOp::onCombineIfPossible(Op* op) {
  if (!DrawOp::onCombineIfPossible(op)) {
    return false;
  }
  auto* that = static_cast<TriangulatingPathOp*>(op);
  if (viewMatrix != that->viewMatrix || localMatrix != that->localMatrix || color != that->color ||
      buffer != nullptr || that->buffer != nullptr) {
    return false;
  }
  vertices.insert(vertices.end(), that->vertices.begin(), that->vertices.end());
  vertexCount += that->vertexCount;
  return true;
}

void TriangulatingPathOp::onPrepare(Gpu* gpu) {
  if (buffer == nullptr) {
    buffer = GpuBuffer::Make(gpu->context(), BufferType::Vertex, vertices.data(),
                             vertices.size() * sizeof(float));
    vertices = {};
  }
}

void TriangulatingPathOp::onExecute(RenderPass* renderPass) {
  if (buffer == nullptr) {
    return;
  }
  auto pipeline = createPipeline(
      renderPass, DefaultGeometryProcessor::Make(color, renderPass->renderTarget()->width(),
                                                 renderPass->renderTarget()->height(), viewMatrix,
                                                 localMatrix));
  renderPass->bindProgramAndScissorClip(pipeline.get(), scissorRect());
  renderPass->bindBuffers(nullptr, buffer);
  renderPass->draw(PrimitiveType::Triangles, 0, vertexCount);
}
}  // namespace tgfx

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

#pragma once

#include <optional>
#include "GLBuffer.h"
#include "GLProgram.h"
#include "gpu/AAType.h"
#include "gpu/Pipeline.h"
#include "gpu/RenderPass.h"
#include "gpu/ops/Op.h"
#include "gpu/processors/FragmentProcessor.h"
#include "gpu/processors/GeometryProcessor.h"
#include "tgfx/core/BlendMode.h"

namespace tgfx {
class VertexArrayObject;

class GLRenderPass : public RenderPass {
 public:
  static std::unique_ptr<GLRenderPass> Make(Context* context);

  void set(std::shared_ptr<RenderTarget> renderTarget,
           std::shared_ptr<Texture> renderTargetTexture);

  void reset();

 protected:
  bool onBindProgramAndScissorClip(const ProgramInfo* programInfo, const Rect& drawBounds) override;
  void onBindBuffers(std::shared_ptr<GpuBuffer> indexBuffer,
                     std::shared_ptr<GpuBuffer> vertexBuffer) override;
  void onDraw(PrimitiveType primitiveType, int baseVertex, int vertexCount) override;
  void onDrawIndexed(PrimitiveType primitiveType, int baseIndex, int indexCount) override;
  void onClear(const Rect& scissor, Color color) override;

 private:
  GLRenderPass(Context* context, std::shared_ptr<VertexArrayObject> vertexArrayObject)
      : RenderPass(context), vertexArrayObject(std::move(vertexArrayObject)) {
  }

  void draw(const std::function<void()>& func);

  std::shared_ptr<VertexArrayObject> vertexArrayObject;
};
}  // namespace tgfx

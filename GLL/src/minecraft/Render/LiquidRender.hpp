//
//  LiquidRender.hpp
//  GLL
//
//  Created by 刘微 on 2019/7/28.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#ifndef LiquidRender_hpp
#define LiquidRender_hpp

#include <stdio.h>
#include <vector>
#include "Render.hpp"
#include "Shader.hpp"
#include "InstanceMeshDrawable.hpp"


namespace GLL {
    class LiquidRender : public Render {
        
    public:
        LiquidRender();
        ~LiquidRender();
        
        void addDrawable(std::shared_ptr<InstanceMeshDrawable> drawable);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clearDrawables();

    private:
        Shader shader;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> drawables;
        void renderInit() override;
    };
}

#endif /* LiquidRender_hpp */

//
//  FloraRender.hpp
//  GLL
//
//  Created by 刘微 on 2019/7/28.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#ifndef FloraRender_hpp
#define FloraRender_hpp

#include <stdio.h>
#include "Render.hpp"
#include "Shader.hpp"
#include "InstanceMeshDrawable.hpp"


namespace GLL {
    class FloraRender : public Render {
        
    public:
        FloraRender();
        ~FloraRender();
        
        void addDrawable(std::shared_ptr<InstanceMeshDrawable> drawable);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        void clearDrawables();

    private:
        Shader shader;
        std::vector<std::shared_ptr<InstanceMeshDrawable>> drawables;
        void renderInit() override;
    };
}

#endif /* FloraRender_hpp */

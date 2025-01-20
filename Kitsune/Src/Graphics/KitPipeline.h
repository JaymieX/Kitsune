#pragma once

namespace Kitsune
{
    class KitPipeline
    {
    public:
        KitPipeline();
        ~KitPipeline();

        KitPipeline(const KitPipeline&) = delete;
        KitPipeline(KitPipeline&&)      = delete;
        
        KitPipeline& operator=(const KitPipeline&) = delete;
        KitPipeline& operator=(KitPipeline&&)      = delete;
    };
}

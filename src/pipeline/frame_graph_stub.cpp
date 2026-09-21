#include "frame_graph_stub.h"
#include <cstdio>
#include <algorithm>

namespace te {

bool FrameGraph::compile(const RenderGraphDesc& desc) {
    std::printf("[Pipeline] FrameGraph compiled with %zu attachments\n", desc.attachments.size());
    for (size_t i = 0; i < desc.attachments.size(); i++) {
        const auto& a = desc.attachments[i];
        std::printf("[Pipeline]   Attachment %zu: %ux%u format=%u\n", i, a.width, a.height, a.format);
    }
    return true;
}

} // namespace te
/*! \file compressor-xor-rle-per-draw-list.cpp
 *  \brief Enter description here.
 */

#include "imgui-ws/imgui-draw-data-compressor.h"

#include "imgui/imgui.h"

namespace {

void writeCmdListToBuffer(const ImDrawList * cmdList, std::vector<char> & buf) {
    uint32_t nVertices = cmdList->VtxBuffer.Size;
    std::copy((char *)(&nVertices), (char *)(&nVertices) + sizeof(nVertices), std::back_inserter(buf));
    std::copy((char *)(cmdList->VtxBuffer.Data), (char *)(cmdList->VtxBuffer.Data) + nVertices*sizeof(ImDrawVert), std::back_inserter(buf));

    uint32_t nIndicesOriginal = cmdList->IdxBuffer.Size;
    uint32_t nIndices = cmdList->IdxBuffer.Size;
    if (nIndicesOriginal % 2 == 1) {
        ++nIndices;
    }
    std::copy((char *)(&nIndices), (char *)(&nIndices) + sizeof(nIndices), std::back_inserter(buf));
    std::copy((char *)(cmdList->IdxBuffer.Data), (char *)(cmdList->IdxBuffer.Data) + nIndicesOriginal*sizeof(ImDrawIdx), std::back_inserter(buf));

    if (nIndicesOriginal % 2 == 1) {
        uint16_t idx = 0;
        std::copy((char *)(&idx), (char *)(&idx) + sizeof(idx), std::back_inserter(buf));
    }

    uint32_t nCmd = cmdList->CmdBuffer.Size;
    std::copy((char *)(&nCmd), (char *)(&nCmd) + sizeof(nCmd), std::back_inserter(buf));

    for (uint32_t iCmd = 0; iCmd < nCmd; iCmd++)
    {
        const ImDrawCmd* pcmd = &cmdList->CmdBuffer[iCmd];

        uint32_t nElements = pcmd->ElemCount;
        std::copy((char *)(&nElements), (char *)(&nElements) + sizeof(nElements), std::back_inserter(buf));

        uint32_t textureId = (uint32_t)(intptr_t)pcmd->TextureId;
        std::copy((char *)(&textureId), (char *)(&textureId) + sizeof(textureId), std::back_inserter(buf));

        uint32_t offsetVtx = (uint32_t)(intptr_t)pcmd->VtxOffset;
        std::copy((char *)(&offsetVtx), (char *)(&offsetVtx) + sizeof(offsetVtx), std::back_inserter(buf));

        uint32_t offsetIdx = (uint32_t)(intptr_t)pcmd->IdxOffset;
        std::copy((char *)(&offsetIdx), (char *)(&offsetIdx) + sizeof(offsetIdx), std::back_inserter(buf));

        auto clipRect = &pcmd->ClipRect;
        std::copy((char *)(clipRect), (char *)(clipRect) + sizeof(ImVec4), std::back_inserter(buf));
    }
}

}
namespace ImDrawDataCompressor {

struct XorRlePerDrawList::Impl {
};

XorRlePerDrawList::XorRlePerDrawList() : m_impl(new Impl()) {}

XorRlePerDrawList::~XorRlePerDrawList() {}

bool XorRlePerDrawList::setDrawData(const struct ImDrawData * drawData) {
    auto & bufs = m_drawListsCur;

    uint32_t nCmdLists = drawData->CmdListsCount;
    bufs.resize(nCmdLists);

    for (uint32_t iList = 0; iList < nCmdLists; iList++) {
        bufs[iList].clear();
        ::writeCmdListToBuffer(drawData->CmdLists[iList], bufs[iList]);
    }

    return true;
}

}

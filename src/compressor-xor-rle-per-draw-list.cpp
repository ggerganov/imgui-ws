/*! \file compressor-xor-rle-per-draw-list.cpp
 *  \brief Enter description here.
 */

#include "imgui-ws/imgui-draw-data-compressor.h"

#include "imgui/imgui.h"

#include <cstring>

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
    m_drawListsPrev = m_drawListsCur;

    uint32_t nCmdLists = drawData->CmdListsCount;
    m_drawListsCur.resize(nCmdLists);

    for (uint32_t iList = 0; iList < nCmdLists; iList++) {
        m_drawListsCur[iList].clear();
        ::writeCmdListToBuffer(drawData->CmdLists[iList], m_drawListsCur[iList]);
    }

    // calculate diff

    m_drawListsDiff.clear();
    m_drawListsDiff.resize(nCmdLists);

    for (uint32_t iList = 0; iList < nCmdLists; ++iList) {
        auto & bufferCur = m_drawListsCur[iList];
        auto & bufferDiff = m_drawListsDiff[iList];

        bufferDiff.clear();

        int32_t type = 1; // Run-Length Encoding
        if (iList >= m_drawListsPrev.size() || m_drawListsPrev[iList].size() != bufferCur.size()) {
            type = 0; // Full update
        }

        std::copy((char *)(&type), (char *)(&type) + sizeof(type), std::back_inserter(bufferDiff));

        if (type == 0) {
            std::copy(bufferCur.begin(), bufferCur.end(), std::back_inserter(bufferDiff));
        } else if (type == 1) {
            auto & bufferPrev = m_drawListsPrev[iList];

            uint32_t a = 0;
            uint32_t b = 0;
            uint32_t c = 0;
            uint32_t n = 0;

            for (int i = 0; i < (int) bufferCur.size(); i += 4) {
                std::memcpy((char *)(&a), bufferPrev.data() + i, sizeof(uint32_t));
                std::memcpy((char *)(&b), bufferCur.data() + i, sizeof(uint32_t));
                a = a ^ b;
                if (a == c) {
                    ++n;
                } else {
                    if (n > 0) {
                        std::copy((char *)(&n), (char *)(&n) + sizeof(uint32_t), std::back_inserter(bufferDiff));
                        std::copy((char *)(&c), (char *)(&c) + sizeof(uint32_t), std::back_inserter(bufferDiff));
                    }
                    n = 1;
                    c = a;
                }
            }

            if (bufferCur.size() % 4 != 0) {
                a = 0;
                b = 0;
                uint32_t i = (bufferCur.size()/4)*4;
                uint32_t k = bufferCur.size() - i;
                std::memcpy((char *)(&a), bufferPrev.data() + i, k);
                std::memcpy((char *)(&b), bufferCur.data() + i, k);
                a = a ^ b;
                if (a == c) {
                    ++n;
                } else {
                    std::copy((char *)(&n), (char *)(&n) + sizeof(uint32_t), std::back_inserter(bufferDiff));
                    std::copy((char *)(&c), (char *)(&c) + sizeof(uint32_t), std::back_inserter(bufferDiff));
                    n = 1;
                    c = a;
                }
            }

            std::copy((char *)(&n), (char *)(&n) + sizeof(uint32_t), std::back_inserter(bufferDiff));
            std::copy((char *)(&c), (char *)(&c) + sizeof(uint32_t), std::back_inserter(bufferDiff));
        }
    }

    return true;
}

}

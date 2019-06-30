/*! \file imgui-ws.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#pragma once

#include "incppect/incppect.h"

// todo : fwd
#include "imgui.h"

#include <map>

class ImGuiWS {
    public:
        using TextureId = uint32_t;

        struct Texture {
            enum Type : int32_t {
                Alpha8 = 0,
                RGBA32,
            };

            std::vector<char> data;
        };

        ImGuiWS() {
        }

        ~ImGuiWS() {
            if (m_worker.joinable()) {
                m_worker.join();
            }
        }

        bool init(int port, const char * pathHttp) {
            Incppect::getInstance().var("imgui.textures[%d]", [this](const auto & idxs) {
                if (m_textures.find(idxs[0]) == m_textures.end()) {
                    return std::string_view { 0, 0 };
                }

                return std::string_view { m_textures[idxs[0]].data.data(), m_textures[idxs[0]].data.size() };
            });

            Incppect::getInstance().var("imgui.draw_data", [this](const auto & ) {
                return std::string_view { m_drawDataBuffer.data(), m_drawDataBuffer.size() };
            });

            m_worker = Incppect::getInstance().runAsync(Incppect::Parameters {
                .portListen = port,
                    .maxPayloadLength_bytes = 256*1024,
                    .httpRoot = pathHttp,
            });

            return m_worker.joinable();
        }

        bool setTexture(TextureId textureId, int32_t width, int32_t height, const char * data) {
            m_textures[textureId].data.resize(sizeof(TextureId) + sizeof(Texture::Type) + 2*sizeof(int32_t) + width*height);

            Texture::Type textureType = Texture::Alpha8;

            size_t offset = 0;
            std::memcpy(m_textures[textureId].data.data() + offset, &textureId, sizeof(textureId)); offset += sizeof(textureId);
            std::memcpy(m_textures[textureId].data.data() + offset, &textureType, sizeof(textureType)); offset += sizeof(textureType);
            std::memcpy(m_textures[textureId].data.data() + offset, &width, sizeof(width)); offset += sizeof(width);
            std::memcpy(m_textures[textureId].data.data() + offset, &height, sizeof(height)); offset += sizeof(height);
            std::memcpy(m_textures[textureId].data.data() + offset, data, width*height);

            return true;
        }

        bool setDrawData(const ImDrawData * drawData) {
            m_drawDataBuffer.clear();

            uint32_t nCmdLists = drawData->CmdListsCount;
            std::copy((char *)(&nCmdLists), (char *)(&nCmdLists) + sizeof(nCmdLists), std::back_inserter(m_drawDataBuffer));

            for (int iList = 0; iList < nCmdLists; iList++) {
                const ImDrawList* cmdList = drawData->CmdLists[iList];

                uint32_t nVertices = cmdList->VtxBuffer.Size;
                std::copy((char *)(&nVertices), (char *)(&nVertices) + sizeof(nVertices), std::back_inserter(m_drawDataBuffer));
                std::copy((char *)(&cmdList->VtxBuffer.Data), (char *)(&cmdList->VtxBuffer.Data) + nVertices*sizeof(ImDrawVert), std::back_inserter(m_drawDataBuffer));

                uint32_t nIndices = cmdList->IdxBuffer.Size;
                std::copy((char *)(&nIndices), (char *)(&nIndices) + sizeof(nIndices), std::back_inserter(m_drawDataBuffer));
                std::copy((char *)(&cmdList->IdxBuffer.Data), (char *)(&cmdList->IdxBuffer.Data) + nIndices*sizeof(ImDrawIdx), std::back_inserter(m_drawDataBuffer));

                uint32_t nCmd = cmdList->CmdBuffer.Size;
                std::copy((char *)(&nCmd), (char *)(&nCmd) + sizeof(nCmd), std::back_inserter(m_drawDataBuffer));

                for (int iCmd = 0; iCmd < nCmd; iCmd++)
                {
                    const ImDrawCmd* pcmd = &cmdList->CmdBuffer[iCmd];

                    uint32_t nElements = pcmd->ElemCount;
                    std::copy((char *)(&nElements), (char *)(&nElements) + sizeof(nElements), std::back_inserter(m_drawDataBuffer));

                    uint32_t textureId = (uint32_t)(intptr_t)pcmd->TextureId;
                    std::copy((char *)(&textureId), (char *)(&textureId) + sizeof(textureId), std::back_inserter(m_drawDataBuffer));

                    uint32_t offsetVtx = (uint32_t)(intptr_t)pcmd->VtxOffset;
                    std::copy((char *)(&offsetVtx), (char *)(&offsetVtx) + sizeof(offsetVtx), std::back_inserter(m_drawDataBuffer));

                    uint32_t offsetIdx = (uint32_t)(intptr_t)pcmd->IdxOffset;
                    std::copy((char *)(&offsetIdx), (char *)(&offsetIdx) + sizeof(offsetIdx), std::back_inserter(m_drawDataBuffer));

                    auto clipRect = &pcmd->ClipRect;
                    std::copy((char *)(clipRect), (char *)(clipRect) + sizeof(ImVec4), std::back_inserter(m_drawDataBuffer));
                }
            }

            static int cnt = 0;
            if (++cnt % 60 == 0) {
                printf("DrawData buffer size = %d bytes\n", (int) m_drawDataBuffer.size());
            }

            return true;
        }

    private:
        std::thread m_worker;

        std::map<TextureId, Texture> m_textures;

        std::vector<char> m_drawDataBuffer;
};

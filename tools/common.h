/*! \file common.h
 *  \brief Saving/loading Dear ImGui DrawData
 */

#pragma once

#include "imgui/imgui.h"
#include "imgui-ws/imgui-ws.h"

#include <vector>
#include <fstream>

struct Session {
    constexpr static auto kHeader = "Dear ImGui DrawData v1.0";

    using FrameData = std::vector<char>;

    bool save(const char * fname) const {
        std::ofstream fs(fname, std::ios::binary);

        fs.write(kHeader, strlen(kHeader));

        uint32_t nFrames = frames.size();
        fs.write((char *)(&nFrames), sizeof(nFrames));

        for (uint32_t i = 0; i < nFrames; ++i) {
            uint32_t frameSize = frames[i].size();
            fs.write((char *)(&frameSize), sizeof(frameSize));
            fs.write((char *)(frames[i].data()), frames[i].size());
        }

        return true;
    }

    bool load(const char * fname) {
        std::ifstream fs(fname, std::ios::binary);
        char header[64];
        std::fill(header, header + 64, 0);
        fs.read(header, strlen(kHeader));
        if (strcmp(header, kHeader)) {
            return false;
        }

        uint32_t nFrames = 0;
        fs.read((char *)(&nFrames), sizeof(nFrames));

        frames.resize(nFrames);
        for (uint32_t i = 0; i < nFrames; ++i) {
            uint32_t frameSize = 0;
            fs.read((char *)(&frameSize), sizeof(frameSize));

            frames[i].resize(frameSize);
            fs.read((char *)(frames[i].data()), frames[i].size());
        }

        return true;
    }

    template<typename T>
    inline void insert(const T & t, std::vector<char> & buf) {
        std::copy((char *)(&t), (char *)(&t) + sizeof(T), std::back_inserter(buf));
    }

    template<>
    inline void insert<ImDrawCmd>(const ImDrawCmd & t, std::vector<char> & buf) {
        insert(t.ElemCount, buf);
        insert(t.ClipRect.x, buf);
        insert(t.ClipRect.y, buf);
        insert(t.ClipRect.z, buf);
        insert(t.ClipRect.w, buf);
        insert(t.TextureId, buf);
        insert(t.VtxOffset, buf);
        insert(t.IdxOffset, buf);
    }

    template<typename T>
    inline void insert(const ImVector<T> & t, std::vector<char> & buf) {
        uint32_t n = t.Size;
        insert(n, buf);
        std::copy((char *)(t.Data), (char *)(t.Data) + n*sizeof(T), std::back_inserter(buf));
    }

    template<>
    inline void insert<ImDrawCmd>(const ImVector<ImDrawCmd> & t, std::vector<char> & buf) {
        uint32_t n = t.Size;
        insert(n, buf);
        for (uint32_t i = 0; i < n; ++i) {
            insert(t[i], buf);
        }
    }

    template<typename T>
    inline void read(T & t, const std::vector<char> & buf, size_t & offset) {
        std::memcpy((char *)(&t), buf.data() + offset, sizeof(T));
        offset += sizeof(T);
    }

    template<>
    inline void read<ImDrawCmd>(ImDrawCmd & t, const std::vector<char> & buf, size_t & offset) {
        read(t.ElemCount, buf, offset);
        read(t.ClipRect.x, buf, offset);
        read(t.ClipRect.y, buf, offset);
        read(t.ClipRect.z, buf, offset);
        read(t.ClipRect.w, buf, offset);
        read(t.TextureId, buf, offset);
        read(t.VtxOffset, buf, offset);
        read(t.IdxOffset, buf, offset);

        t.UserCallback = NULL;
    }

    template<typename T>
    inline void read(ImVector<T> & t, std::vector<char> & buf, size_t & offset) {
        uint32_t n = 0;
        read(n, buf, offset);
        t.resize(n);
        std::memcpy((char *)(t.Data), buf.data() + offset, n*sizeof(T));
        offset += n*sizeof(T);
    }

    template<>
    inline void read<ImDrawCmd>(ImVector<ImDrawCmd> & t, std::vector<char> & buf, size_t & offset) {
        uint32_t n = 0;
        read(n, buf, offset);
        t.resize(n);
        for (uint32_t i = 0; i < n; ++i) {
            read(t[i], buf, offset);
        }
    }

    bool addFrame(const ImDrawData * drawData) {
        FrameData frame;

        insert(drawData->Valid, frame);
        insert(drawData->CmdListsCount, frame);
        insert(drawData->TotalIdxCount, frame);
        insert(drawData->TotalVtxCount, frame);
        insert(drawData->DisplayPos.x, frame);
        insert(drawData->DisplayPos.y, frame);
        insert(drawData->DisplaySize.x, frame);
        insert(drawData->DisplaySize.y, frame);
        insert(drawData->FramebufferScale.x, frame);
        insert(drawData->FramebufferScale.y, frame);

        for (int32_t iList = 0; iList < drawData->CmdListsCount; ++iList) {
            auto & cmdList = drawData->CmdLists[iList];

            insert(cmdList->CmdBuffer, frame);
            insert(cmdList->VtxBuffer, frame);
            insert(cmdList->IdxBuffer, frame);
            insert(cmdList->Flags, frame);
        }

        frames.emplace_back(std::move(frame));

        return true;
    }

    bool getFrame(int32_t fid, ImDrawData * drawData, std::vector<ImDrawList> & drawLists, const ImDrawListSharedData * drawListSharedData) {
        if (fid >= (int32_t) frames.size()) return false;

        size_t offset = 0;
        auto & buf = frames[fid];


        read(drawData->Valid, buf, offset);
        read(drawData->CmdListsCount, buf, offset);
        read(drawData->TotalIdxCount, buf, offset);
        read(drawData->TotalVtxCount, buf, offset);
        read(drawData->DisplayPos.x, buf, offset);
        read(drawData->DisplayPos.y, buf, offset);
        read(drawData->DisplaySize.x, buf, offset);
        read(drawData->DisplaySize.y, buf, offset);
        read(drawData->FramebufferScale.x, buf, offset);
        read(drawData->FramebufferScale.y, buf, offset);

        if ((int) drawLists.size() < drawData->CmdListsCount) {
            drawLists.resize(drawData->CmdListsCount, ImDrawList(drawListSharedData));
        }


        if (drawData->CmdLists) {
            delete [] drawData->CmdLists;
        }

        drawData->CmdLists = new ImDrawList* [drawData->CmdListsCount];

        for (int32_t iList = 0; iList < drawData->CmdListsCount; ++iList) {
            drawData->CmdLists[iList] = &drawLists[iList];

            auto & cmdList = drawData->CmdLists[iList];

            read(cmdList->CmdBuffer, buf, offset);
            read(cmdList->VtxBuffer, buf, offset);
            read(cmdList->IdxBuffer, buf, offset);
            read(cmdList->Flags, buf, offset);
        }

        return true;
    }

    int32_t nFrames() const {
        return frames.size();
    }

    uint64_t totalSize_bytes() const {
        uint64_t result = 0;

        for (auto & frame : frames) {
            result += frame.size();
        }

        return result;
    }

    void printInfo() const {
        printf("    - Total frames      = %d\n", (int) frames.size());
        printf("    - Total size        = %d bytes\n", (int) totalSize_bytes());
    }

    std::vector<FrameData> frames;
};

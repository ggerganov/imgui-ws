/*! \file imgui-draw-data-compressor.h
 *  \brief Enter description here.
 */

#pragma once

#include <cstdint>
#include <vector>
#include <memory>

struct ImDrawData;

namespace ImDrawDataCompressor {

class Interface {
public:
    using DrawList = std::vector<char>;
    using DrawLists = std::vector<DrawList>;
    using DrawListDiff = std::vector<char>;
    using DrawListsDiff = std::vector<DrawListDiff>;

    Interface() {}
    virtual ~Interface() {}

    virtual bool setDrawData(const ::ImDrawData * drawData) = 0;

    virtual DrawLists & getDrawLists() {
        return m_drawListsCur;
    }

    virtual DrawListsDiff & getDrawListsDiff() {
        return m_drawListsDiff;
    }

    virtual uint64_t diffSize() const {
        uint64_t res = 0;

        for (auto & list : m_drawListsDiff) {
            res += list.size();
        }

        return res;
    }

protected:
    DrawLists m_drawListsCur;
    DrawLists m_drawListsPrev;
    DrawListsDiff m_drawListsDiff;
};

class XorRlePerDrawList : public Interface {
public:
    static constexpr auto kName = "XorRlePerDrawList";

    XorRlePerDrawList();
    virtual ~XorRlePerDrawList();

    virtual bool setDrawData(const ::ImDrawData * drawData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

class XorRlePerDrawListWithVtxOffset : public Interface {
public:
    static constexpr auto kName = "XorRlePerDrawListWithVtxOffset";

    XorRlePerDrawListWithVtxOffset();
    virtual ~XorRlePerDrawListWithVtxOffset();

    virtual bool setDrawData(const ::ImDrawData * drawData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

}

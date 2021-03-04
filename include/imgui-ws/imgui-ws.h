/*! \file imgui-ws.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#pragma once

#include <vector>
#include <queue>
#include <string>
#include <memory>

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

        struct Event {
            enum Type {
                Unknown = 0,
                Connected,
                Disconnected,
                MouseMove,
                MouseDown,
                MouseUp,
                MouseWheel,
                KeyPress,
                KeyDown,
                KeyUp,
            };

            Type type = Unknown;

            int32_t clientId = -1;

            float mouse_x = 0.0f;
            float mouse_y = 0.0f;

            float wheel_x = 0.0f;
            float wheel_y = 0.0f;

            int32_t mouse_but = 0;

            int32_t key = 0;

            std::string ip;
        };

        ImGuiWS();
        ~ImGuiWS();

        bool init(int32_t port, const char * pathHttp);
        bool setTexture(TextureId textureId, int32_t width, int32_t height, const char * data);
        bool setDrawData(const struct ImDrawData * drawData);
        void stop();

        int32_t nConnected() const;

        std::deque<Event> takeEvents();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

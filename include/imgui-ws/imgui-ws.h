/*! \file imgui-ws.h
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <thread>
#include <shared_mutex>
#include <queue>
#include <string>

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
            };

            Type type = Unknown;

            int32_t clientId = -1;

            float mouse_x = 0.0;
            float mouse_y = 0.0;

            int mouse_but = 0;

            std::string ip;
        };

        struct Events {
            std::deque<Event> data;

            std::mutex mutex;
            std::condition_variable cv;

            void push(Event && event) {
                std::lock_guard<std::mutex> lock(mutex);
                data.push_back(std::move(event));
                cv.notify_one();
            }
        };

        ImGuiWS();
        ~ImGuiWS();

        bool init(int port, const char * pathHttp);
        bool setTexture(TextureId textureId, int32_t width, int32_t height, const char * data);
        bool setDrawData(const struct ImDrawData * drawData);

        std::deque<Event> && takeEvents() {
            std::lock_guard<std::mutex> lock(m_events.mutex);
            return std::move(m_events.data);
        }

    private:
        std::thread m_worker;
        mutable std::shared_mutex m_mutex;

        struct Data {
            std::map<TextureId, Texture> textures;
            std::vector<char> drawDataBuffer;
        };

        Data m_dataWrite;
        Data m_dataRead;

        Events m_events;
};

/*! \file imgui-ws.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "imgui-ws/imgui-ws.h"
#include "imgui-ws/imgui-draw-data-compressor.h"

#include "common.h"

#include "incppect/incppect.h"

#include <atomic>
#include <map>
#include <thread>
#include <sstream>
#include <cstring>
#include <mutex>
#include <shared_mutex>

// not using ssl
using incppect = Incppect<false>;

struct ImGuiWS::Impl {
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

    struct Data {
        std::map<TextureId, Texture> textures;

        ImDrawDataCompressor::Interface::DrawLists drawLists;
        ImDrawDataCompressor::Interface::DrawListsDiff drawListsDiff;
    };

    Impl() : compressorDrawData(new ImDrawDataCompressor::XorRlePerDrawListWithVtxOffset()) {}

    std::atomic<int32_t> nConnected = 0;

    std::thread worker;
    mutable std::shared_mutex mutex;

    Data dataWrite;
    Data dataRead;

    Events events;

    incppect incpp;

    std::unique_ptr<ImDrawDataCompressor::Interface> compressorDrawData;
};

ImGuiWS::ImGuiWS() : m_impl(new Impl()) {
}

ImGuiWS::~ImGuiWS() {
    m_impl->incpp.stop();
    if (m_impl->worker.joinable()) {
        m_impl->worker.join();
    }
}

bool ImGuiWS::addVar(const TPath & path, TGetter && getter) {
    return m_impl->incpp.var(path, std::move(getter));
}

bool ImGuiWS::init(int32_t port, const char * pathHttp) {
    m_impl->incpp.var("my_id[%d]", [](const auto & idxs) {
        static int32_t id;
        id = idxs[0];
        return incppect::view(id);
    });

    // get texture by id
    // todo : needs some work to support more than 1 texture
    m_impl->incpp.var("imgui.textures[%d]", [this](const auto & idxs) {
        static std::vector<char> data;
        {
            std::shared_lock lock(m_impl->mutex);
            if (m_impl->dataRead.textures.find(idxs[0]) == m_impl->dataRead.textures.end()) {
                return std::string_view { 0, 0 };
            }
            data.clear();
            std::copy(m_impl->dataRead.textures[idxs[0]].data.data(), m_impl->dataRead.textures[idxs[0]].data.data() + m_impl->dataRead.textures[idxs[0]].data.size(),
                      std::back_inserter(data));
        }

        return std::string_view { data.data(), data.size() };
    });

    // get imgui's draw data
    m_impl->incpp.var("imgui.n_draw_lists", [this](const auto & ) {
        std::shared_lock lock(m_impl->mutex);

        return incppect::view(m_impl->dataRead.drawLists.size());
    });

    m_impl->incpp.var("imgui.draw_list[%d]", [this](const auto & idxs) {
        static std::vector<char> data;
        {
            std::shared_lock lock(m_impl->mutex);

            if (idxs[0] >= (int) m_impl->dataRead.drawLists.size()) {
                return std::string_view { nullptr, 0 };
            }

            data.clear();
            std::copy(m_impl->dataRead.drawLists[idxs[0]].data(),
                      m_impl->dataRead.drawLists[idxs[0]].data() + m_impl->dataRead.drawLists[idxs[0]].size(),
                      std::back_inserter(data));
        }

        return std::string_view { data.data(), data.size() };
    });

    m_impl->incpp.handler([&](int clientId, incppect::EventType etype, std::string_view data) {
        Event event;

        event.clientId = clientId;

        switch (etype) {
            case incppect::Connect:
                {
                    ++m_impl->nConnected;
                    event.type = Event::Connected;
                    std::stringstream ss;
                    { int a = data[0]; if (a < 0) a += 256; ss << a << "."; }
                    { int a = data[1]; if (a < 0) a += 256; ss << a << "."; }
                    { int a = data[2]; if (a < 0) a += 256; ss << a << "."; }
                    { int a = data[3]; if (a < 0) a += 256; ss << a; }
                    event.ip = ss.str();
                }
                break;
            case incppect::Disconnect:
                {
                    --m_impl->nConnected;
                    event.type = Event::Disconnected;
                }
                break;
            case incppect::Custom:
                {
                    std::stringstream ss;
                    ss << data;

                    int type = -1;
                    ss >> type;

                    //printf("Received event %d '%s'\n", type, ss.str().c_str());
                    switch (type) {
                        case 0:
                            {
                                // mouse move
                                event.type = Event::MouseMove;
                                ss >> event.mouse_x >> event.mouse_y;
                                //printf("    mouse %g %g\n", event.mouse_x, event.mouse_y);
                            }
                            break;
                        case 1:
                            {
                                // mouse down
                                event.type = Event::MouseDown;
                                ss >> event.mouse_but >> event.mouse_x >> event.mouse_y;
                                //printf("    mouse %d down\n", event.mouse_but);
                            }
                            break;
                        case 2:
                            {
                                // mouse up
                                event.type = Event::MouseUp;
                                ss >> event.mouse_but >> event.mouse_x >> event.mouse_y;
                                //printf("    mouse %d up\n", event.mouse_but);
                            }
                            break;
                        case 3:
                            {
                                // mouse wheel
                                event.type = Event::MouseWheel;
                                ss >> event.wheel_x >> event.wheel_y;
                            }
                            break;
                        case 4:
                            {
                                // key press
                                event.type = Event::KeyPress;
                                ss >> event.key;
                            }
                            break;
                        case 5:
                            {
                                // key down
                                event.type = Event::KeyDown;
                                ss >> event.key;
                            }
                            break;
                        case 6:
                            {
                                // key up
                                event.type = Event::KeyUp;
                                ss >> event.key;
                            }
                            break;
                        default:
                            {
                                printf("Unknown input received from client: id = %d, type = %d\n", clientId, type);
                                return;
                            }
                            break;
                    };
                }
                break;
        };

        m_impl->events.push(std::move(event));
    });

    m_impl->incpp.setResource("/imgui-ws.js", kImGuiWS_js);

    // start the http/websocket server
    incppect::Parameters parameters;
    parameters.portListen = port;
    parameters.maxPayloadLength_bytes = 1024*1024;
    parameters.tLastRequestTimeout_ms = -1;
    parameters.httpRoot = pathHttp;
    parameters.sslKey = "key.pem";
    parameters.sslCert = "cert.pem";
    m_impl->worker = m_impl->incpp.runAsync(parameters);

    return m_impl->worker.joinable();
}

bool ImGuiWS::setTexture(TextureId textureId, int32_t width, int32_t height, const char * data) {
    m_impl->dataWrite.textures[textureId].data.resize(sizeof(TextureId) + sizeof(Texture::Type) + 2*sizeof(int32_t) + width*height);

    Texture::Type textureType = Texture::Alpha8;

    size_t offset = 0;
    std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &textureId, sizeof(textureId)); offset += sizeof(textureId);
    std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &textureType, sizeof(textureType)); offset += sizeof(textureType);
    std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &width, sizeof(width)); offset += sizeof(width);
    std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &height, sizeof(height)); offset += sizeof(height);
    std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, data, width*height);

    {
        std::unique_lock lock(m_impl->mutex);
        m_impl->dataRead.textures[textureId] = m_impl->dataWrite.textures[textureId];
    }

    return true;
}

bool ImGuiWS::setDrawData(const ImDrawData * drawData) {
    bool result = true;

    result &= m_impl->compressorDrawData->setDrawData(drawData);

    auto & drawLists = m_impl->compressorDrawData->getDrawLists();
    auto & drawListsDiff = m_impl->compressorDrawData->getDrawListsDiff();

    // make the draw lists available to incppect clients
    {
        std::unique_lock lock(m_impl->mutex);

        m_impl->dataRead.drawLists = std::move(drawLists);
        m_impl->dataRead.drawListsDiff = std::move(drawListsDiff);
    }

    return true;
}

int32_t ImGuiWS::nConnected() const {
    return m_impl->nConnected;
}

std::deque<ImGuiWS::Event> ImGuiWS::takeEvents() {
    std::lock_guard<std::mutex> lock(m_impl->events.mutex);
    auto res = std::move(m_impl->events.data);
    return res;
}

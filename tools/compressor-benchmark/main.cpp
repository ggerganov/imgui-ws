#include "common.h"

#include "imgui-ws/imgui-draw-data-compressor.h"

#include <vector>
#include <string>

template <typename T>
bool benchmark(const std::vector<std::string> & fnameSessions) {
    printf("[+] Running benchmark for compressor '%s'\n", T::kName);

    for (int i = 0; i < (int) fnameSessions.size(); ++i) {
        auto & fname = fnameSessions[i];

        printf("[+] Processing session '%s' ...\n", fname.c_str());

        Session session;
        if (session.load(fname.c_str()) == false) {
            fprintf(stderr, "    [E] Failed reading the file. Probably not an ImDrawData session file?\n");
            continue;
        }

        int fid = 0;
        ImDrawData drawData;
        std::vector<ImDrawList> drawLists;

        T compressor;
        uint64_t compressedSize_bytes = 0;
        while (session.getFrame(fid, &drawData, drawLists, ImGui::GetDrawListSharedData())) {
            if (compressor.setDrawData(&drawData) == false) {
                fprintf(stderr, "    [E] Failed setting draw data for frame %d\n", fid);
                break;
            }

            compressedSize_bytes += compressor.diffSize();

            ++fid;
        }

        printf("    - Compressed size   = %d bytes\n", (int) compressedSize_bytes);
        printf("    - Compression ratio = %6.4f times\n", ((double)(session.totalSize_bytes()))/compressedSize_bytes);
        printf("    - Average bandwidth = %6.4f kB/s (at 60.0 fps)\n", ((double)(compressedSize_bytes))/1024.0*(60.0/session.nFrames()));
        printf("\n");
    }

    return true;
}

int main(int argc , char ** argv) {
    printf("Usage: %s session0.imdd [session1.imdd] [session2.imdd] [...]\n", argv[0]);

    if (argc < 2) {
        return -1;
    }

    std::vector<std::string> fnameSessions;

    for (int i = 1; i < argc; ++i) {
        printf("[+] Session '%s' info:\n", argv[i]);
        Session session;
        if (session.load(argv[i]) == false) {
            fprintf(stderr, "    [E] Failed reading the file. Probably not an ImDrawData session file?\n");
        } else {
            session.printInfo();
            fnameSessions.push_back(argv[i]);
        }
    }
    printf("\n");

    if (fnameSessions.size() == 0) {
        fprintf(stderr, "[E] No valid files provided - nothing to do\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    benchmark<ImDrawDataCompressor::XorRlePerDrawList>(fnameSessions);
    benchmark<ImDrawDataCompressor::XorRlePerDrawListWithVtxOffset>(fnameSessions);

    ImGui::DestroyContext();

    return 0;
}

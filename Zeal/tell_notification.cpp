#include "Zeal.h"
#include "tell_notification.h"
#include <Windows.h>
#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

DWORD getWavDuration(std::filesystem::path soundPath) {
    HMMIO hmmio;
    MMCKINFO parentChunk;
    MMCKINFO subChunk;
    WAVEFORMATEX wfx;

    hmmio = mmioOpen((LPWSTR)soundPath.c_str(), NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio) {
        std::cerr << "Failed to open wav file." << std::endl;
        return 0;
    }

    parentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, &parentChunk, NULL, MMIO_FINDRIFF)) {
        std::cerr << "Failed to find WAVE chunk." << std::endl;
        mmioClose(hmmio, 0);
        return 0;
    }

    subChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &subChunk, &parentChunk, MMIO_FINDCHUNK)) {
        std::cerr << "Failed to find fmt chunk." << std::endl;
        mmioClose(hmmio, 0);
        return 0;
    }

    if (mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx)) != sizeof(wfx)) {
        std::cerr << "Failed to read fmt chunk." << std::endl;
        mmioClose(hmmio, 0);
        return 0;
    }

    mmioAscend(hmmio, &subChunk, 0);

    subChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &subChunk, &parentChunk, MMIO_FINDCHUNK)) {
        std::cerr << "Failed to find data chunk." << std::endl;
        mmioClose(hmmio, 0);
        return 0;
    }

    DWORD dataSize = subChunk.cksize;
    mmioClose(hmmio, 0);

    DWORD duration = (dataSize * 1000) / wfx.nAvgBytesPerSec;
    return duration;
}

void playSound(std::filesystem::path soundFile, int32_t duration)
{
	PlaySound(soundFile.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void TellNotification::AddOutputText(Zeal::EqUI::ChatWnd*& wnd, std::string msg, short& channel)
{
	if (channel == USERCOLOR_TELL)
	{
		std::filesystem::path wav_filepath = std::filesystem::current_path() / "uifiles\\zeal\\sounds\\notification.wav";
        int32_t duration = getWavDuration(wav_filepath);
        if (duration > 0)
        {
            std::thread soundThread(playSound, wav_filepath, duration);
            soundThread.detach();
        }
	}
}

TellNotification::TellNotification(ZealService* zeal)
{
	zeal->callbacks->AddOutputText([this](Zeal::EqUI::ChatWnd*& wnd, std::string msg, short& channel) { this->AddOutputText(wnd, msg, channel); });
}

TellNotification::~TellNotification()
{
}
#include "Zeal.h"
#include "tell_notification.h"
#include <Windows.h>
#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

void TellNotification::AddOutputText(Zeal::EqUI::ChatWnd*& wnd, std::string msg, short& channel)
{
	if (channel == USERCOLOR_TELL)
	{
		std::filesystem::path wav_filepath = std::filesystem::current_path() / "uifiles\\zeal\\sounds\\notification.wav";
		PlaySound(wav_filepath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
}

TellNotification::TellNotification(ZealService* zeal)
{
	zeal->callbacks->AddOutputText([this](Zeal::EqUI::ChatWnd*& wnd, std::string msg, short& channel) { this->AddOutputText(wnd, msg, channel); });
}

TellNotification::~TellNotification()
{
}
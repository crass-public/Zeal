#pragma once

class TellNotification
{
public:
	TellNotification(class ZealService* zeal);
	~TellNotification();
	void AddOutputText(Zeal::EqUI::ChatWnd*& wnd, std::string msg, short& channel);
};
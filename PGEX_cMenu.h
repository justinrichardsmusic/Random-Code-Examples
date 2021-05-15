#pragma once
#include "olcPixelGameEngine.h"

struct panel
{
	panel(std::string name)
	{
		panelName = name;
	}

	std::string panelName = "";
	bool bEnabled = true;
	bool bSelected = false;
};


class cMenu
{
public:
	bool bVisible = true;
	bool bActive = true;
	std::string strCalledByPanel = "";

	std::vector<panel> panels;
	olc::vf2d vecMenuPos;
	olc::vf2d vecPanelSize;
	olc::vf2d vecPanelOffset;
	olc::vf2d vecTextOffset = { 8.0f, 8.0f };

	olc::Sprite* sprTitle = nullptr;
	olc::Decal* decTitle = nullptr;
	olc::vf2d vecTitlePos;

	olc::Pixel pPanelColour = olc::VERY_DARK_GREY;
	olc::Pixel pPanelHighlight = olc::DARK_GREY;
	olc::Pixel pTextColour = olc::WHITE;
	olc::Pixel pTextHighlight = olc::YELLOW;

	float fTintRed = 1.0f;
	float fTintGreen = 1.0f;
	float fTintBlue = 1.0f;
	float fTintAlpha = 1.0f;

	int nFontSize = 1;

	void SetMenu(std::vector<std::string> names, olc::vf2d pos, olc::vf2d size, olc::vf2d panelOffset, std::string strSpriteTitle, olc::vf2d titlePos);
	void AddNewPanel(std::string name, std::string nameOfPanelNeighbour, bool addAfter = true);
	void RemovePanel(std::string name);
	void FormatMenu(olc::Pixel panelColour, olc::Pixel panelHighlight, olc::Pixel textColour, olc::Pixel textHighlight, int fontSize);
	void DrawMenu(olc::PixelGameEngine* pge);
	olc::Pixel Tint(olc::Pixel originalColour);

	panel& GetPanel(std::string name);
	void GetSelectedPanelName(std::string& name);
	bool MouseHover(olc::vf2d vecMouse, int i);
	void SetActive(bool active = true, bool visible = true);
};

void cMenu::SetMenu(std::vector<std::string> names, olc::vf2d pos, olc::vf2d size, olc::vf2d panelOffset, std::string strSpriteTitle, olc::vf2d titlePos)
{
	sprTitle = new olc::Sprite(strSpriteTitle);
	decTitle = new olc::Decal(sprTitle);
	vecTitlePos = titlePos;

	vecMenuPos = pos;
	vecPanelSize = size;
	vecPanelOffset = panelOffset;
	vecTextOffset = { (float)nFontSize * 4.0f, (float)nFontSize * 4.0f };

	for (auto& n : names)
		panels.push_back(n);
}

void cMenu::AddNewPanel(std::string name, std::string nameOfPanelNeighbour, bool addAfter)
{
	int i = 1;
	for (auto& p : panels)
	{
		if (p.panelName == nameOfPanelNeighbour)
		{
			addAfter ? panels.emplace(panels.begin() + i, name) : panels.emplace(panels.begin() + i - 1, name);
			break;
		}
		else
			i++;
	}
}

void cMenu::RemovePanel(std::string name)
{
	int i = 0;
	for (auto& p : panels)
	{
		if (p.panelName == name)
		{
			panels.erase(panels.begin() + i);
			break;
		}
		else
			i++;
	}
}

void cMenu::FormatMenu(olc::Pixel panelColour, olc::Pixel panelHighlight, olc::Pixel textColour, olc::Pixel textHighlight, int fontSize)
{
	pPanelColour = panelColour;
	pPanelHighlight = panelHighlight;
	pTextColour = textColour;
	pTextHighlight = textHighlight;
	nFontSize = fontSize;
	vecTextOffset = { (float)nFontSize * 4.0f, (float)nFontSize * 4.0f };

}

void cMenu::DrawMenu(olc::PixelGameEngine* pge)
{
	int i = 0;
	olc::vf2d vecShadowOffset = { 2.0f, 2.0f };

	pge->DrawDecal(vecTitlePos, decTitle, { 1.0f, 1.0f }, olc::Pixel(255.0f * fTintRed, 255.0f * fTintGreen, 255.0f * fTintBlue, 255.0f * fTintAlpha));
	for (auto& p : panels)
	{
		p.bSelected = false;
		p.bSelected = bActive && p.bEnabled ? MouseHover({ (float)pge->GetMouseX(), (float)pge->GetMouseY() }, i) : false;

		if (p.panelName != "")
		{
			pge->FillRect(vecMenuPos + (vecPanelOffset * i), vecPanelSize, p.bSelected && p.bEnabled ? Tint(pPanelHighlight) : Tint(pPanelColour));
			pge->DrawString(vecMenuPos + (vecPanelOffset * i) + (vecTextOffset + vecShadowOffset), p.panelName, olc::BLACK, nFontSize);
			if (p.bEnabled) pge->DrawString(vecMenuPos + (vecPanelOffset * i) + vecTextOffset, p.panelName, p.bSelected ? Tint(pTextHighlight) : Tint(pTextColour), nFontSize);
		}
		i++;
	}
}

olc::Pixel cMenu::Tint(olc::Pixel originalColour)
{
	olc::Pixel newColour = originalColour;

	newColour.r *= fTintRed;
	newColour.g *= fTintGreen;
	newColour.b *= fTintBlue;
	newColour.a *= fTintAlpha;

	return newColour;
}

panel& cMenu::GetPanel(std::string name)
{
	for (auto& p : panels)
		if (p.panelName == name) return p;

	return panels.back(); // have to return some reference if the string doesn't match
}

void cMenu::GetSelectedPanelName(std::string& name)
{
	for (auto& p : panels)
		if (p.bSelected)
		{
			name = p.panelName;
			break;
		}

	return;
}

bool cMenu::MouseHover(olc::vf2d vecMouse, int i)
{
	if (vecMouse.x > vecMenuPos.x + (vecPanelOffset.x * i) &&
		vecMouse.x < vecMenuPos.x + (vecPanelOffset.x * i) + vecPanelSize.x &&
		vecMouse.y > vecMenuPos.y + (vecPanelOffset.y * i) &&
		vecMouse.y < vecMenuPos.y + (vecPanelOffset.y * i) + vecPanelSize.y) return true;

	return false;
}

void cMenu::SetActive(bool active, bool visible)
{
	bActive = active;
	bVisible = visible;

	if (bActive)
	{
		fTintRed = 1.0f;
		fTintGreen = 1.0f;
		fTintBlue = 1.0f;
		fTintAlpha = 1.0f;
	}
	else
	{
		fTintRed = 0.5f;
		fTintGreen = 0.5f;
		fTintBlue = 0.5f;
		fTintAlpha = 1.0f;
	}
}

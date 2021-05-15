#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "olcPGEX_cMenu.h"

class MenuGeneric : public olc::PixelGameEngine
{
public:
	MenuGeneric()
	{
		sAppName = "Generic Menu System for PGE 2.0";
	}

	std::vector<std::string> menuMainNames = { "New Game", "Continue", "Select Track", "", "Exit" };
	cMenu menuMain;

	std::vector<std::string> menuYesNoNames = { "Yes", "No" };
	cMenu menuYesNo;


public:
	bool OnUserCreate() override
	{

		menuMain.SetMenu(menuMainNames, { 64.0f, 140.0f }, { 318.0f, 48.0f }, { -8.0f, 64.0f }, "./assets/MenuData/menuMainTitle.png", { 32.0f, 32.0f });
		menuMain.GetPanel("Continue").bEnabled = false;

		menuMain.AddFaderToPanel("Red", menuMain.GetPanel("New Game"), { 318.0f + 32.0f, 8.0f }, { 512.0f, 32.0f }, 0.0f, 255.0f);
		menuMain.GetPanel("New Game").fader.fValue = 192.0f;
		menuMain.GetPanel("New Game").fader.faderColour = olc::Pixel(192, 0, 0);

		menuMain.AddFaderToPanel("Green", menuMain.GetPanel("Continue"), { 318.0f + 32.0f, 8.0f }, { 512.0f, 32.0f }, 0.0f, 255.0f);
		menuMain.GetPanel("Continue").fader.fValue = 255.0f;
		menuMain.GetPanel("Continue").fader.faderColour = olc::Pixel(0, 255, 0);

		menuMain.AddFaderToPanel("Blue", menuMain.GetPanel("Select Track"), { 318.0f + 32.0f, 8.0f }, { 512.0f, 32.0f }, 0.0f, 255.0f);
		menuMain.GetPanel("Select Track").fader.fValue = 63.0f;
		menuMain.GetPanel("Select Track").fader.faderColour = olc::Pixel(0, 0, 63);

		//menuMain.FormatMenu(olc::Pixel(0, 27, 63), olc::Pixel(0, 63, 95), olc::Pixel(95, 159, 191), olc::Pixel(159, 191, 255), 3);
		menuMain.FormatMenu(olc::VERY_DARK_GREY, olc::DARK_GREY, olc::WHITE, olc::YELLOW, 3);

		menuYesNo.SetMenu(menuYesNoNames, { (float)ScreenWidth() * 0.5f - 80.0f, (float)ScreenHeight() * 0.5f + 16.0f }, { 64.0f, 32.0f }, { 96.0f, 0.0f }, "./assets/MenuData/menuYesNo.png", { (float)ScreenWidth() * 0.5f - 225.0f, (float)ScreenHeight() * 0.5f - 82.0f });
		//menuYesNo.FormatMenu(olc::Pixel(0, 27, 63), olc::Pixel(0, 63, 95), olc::Pixel(95, 159, 191), olc::Pixel(159, 191, 255), 2);
		menuYesNo.FormatMenu(olc::VERY_DARK_GREY, olc::DARK_GREY, olc::WHITE, olc::YELLOW, 2);
		menuYesNo.SetActive(false, false);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLANK);

		std::string strSelectedPanel = "";

		if (menuMain.bActive)
		{
			menuMain.GetSelectedPanelName(strSelectedPanel);
			if (GetMouse(0).bReleased)
			{
				if (strSelectedPanel == "New Game")
				{
					menuMain.GetPanel("New Game").bEnabled = false;
					menuMain.GetPanel("Select Track").bEnabled = false;

					menuMain.GetPanel("Continue").bEnabled = true;

					menuMain.AddNewPanel("End Game", "Select Track");
				}
				else if (strSelectedPanel == "End Game")
				{
					menuMain.SetActive(false);

					menuYesNo.strCalledByPanel = strSelectedPanel;
					menuYesNo.SetActive();

				}
				else if (strSelectedPanel == "Exit")
				{
					menuMain.SetActive(false);

					menuYesNo.strCalledByPanel = strSelectedPanel;
					menuYesNo.SetActive();
				}
			}
		}
		else if (menuYesNo.bActive)
		{
			menuYesNo.GetSelectedPanelName(strSelectedPanel);
			if (GetMouse(0).bReleased)
			{
				if (strSelectedPanel == "No")
				{
					if (menuYesNo.strCalledByPanel == "Exit" || menuYesNo.strCalledByPanel == "End Game")
					{
						menuMain.SetActive();

						menuYesNo.SetActive(false, false);
						menuYesNo.strCalledByPanel = "";
					}
				}
				else if (strSelectedPanel == "Yes")
				{
					if (menuYesNo.strCalledByPanel == "Exit") return false; // Terminate Program

					if (menuYesNo.strCalledByPanel == "End Game")
					{
						menuMain.GetPanel("New Game").bEnabled = true;
						menuMain.GetPanel("Select Track").bEnabled = true;

						menuMain.GetPanel("Continue").bEnabled = false;

						menuMain.RemovePanel("End Game");
						menuMain.SetActive();

						menuYesNo.SetActive(false, false);
						menuYesNo.strCalledByPanel = "";
					}
				}
			}
		}

		if (menuMain.bVisible) menuMain.DrawMenu(this);
		if (menuYesNo.bVisible) menuYesNo.DrawMenu(this);

		//DrawCircle(ScreenWidth() * 0.5, ScreenHeight() * 0.5, abs(GetMouseX() - ScreenWidth() * 0.5), olc::RED);

		return true;
	}
};

int main()
{
	FreeConsole();

	MenuGeneric mnu;
	if (mnu.Construct(960, 720, 1, 1))
		mnu.Start();

	return 0;
}
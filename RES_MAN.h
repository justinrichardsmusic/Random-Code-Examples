#pragma once
#include "olcPixelGameEngine.h"
#include <deque>

#ifndef RES_MAN_SINGLE_INSTANCE
#define RES_MAN_SINGLE_INSTANCE

class RES_MAN : public olc::PGEX
{
public:
	RES_MAN(const RES_MAN&) = delete;
	RES_MAN operator=(const RES_MAN&) = delete;

	static RES_MAN& GetInstance() 
	{
		static RES_MAN s_Instance;
		return s_Instance; 
	}

private:
	RES_MAN() {};

	struct resImage
	{
		resImage(uint8_t id, std::string fName)
		{
			ID = id;
			fileName = fName;
			Load(fName);
		}

		bool operator== (const uint8_t& id) const { return id == this->ID; }

		uint8_t ID = -1;
		std::string fileName = "";
		olc::Renderable rdb;
		bool bIsLoaded = false;

		bool Load(std::string fName) { if (rdb.Load(fName))	bIsLoaded = true; return bIsLoaded;	}
	};

	struct resSound
	{
		uint8_t ID = -1;
		std::string fileName = "";
		// TO DO - implement audio
		bool bIsLoaded = false;
	};

	std::deque<resImage> i_resImages;
	std::deque<resSound> i_resSounds;

	olc::Renderable* i_Image(const uint8_t imageID, const std::string imageFileName)
	{
		for (auto& i : i_resImages)										// Check for existing imageID in our image list
			if (i.ID == imageID)
			{
				if (i.bIsLoaded)										// Image is already loaded
					return &i.rdb;

				if (imageFileName == "")								// Image needs to be loaded
				{
					if (i.fileName == "")								// No file name to load from
						return nullptr;
					else
					{
						if (i.Load(i.fileName))							// No file name supplied, fall back to existing file name
							return &i.rdb;

					}
				}
				else
				{
					if (i.Load(imageFileName))							// Load image using file name supplied
						return &i.rdb;

				}

				return nullptr;											// Image could not be loaded (fail safe)
			}

		if (imageFileName == "")										// If no matching ID and empty file name we have nothing to add to our list, return nullptr
			return nullptr;

		i_resImages.emplace_back( resImage(imageID, imageFileName));	// Add our newly loaded image to our image list

		if (i_resImages.size() > 0)										// Return the renderable as long as it was successfully loaded
			if (i_resImages.back().bIsLoaded)
				return &i_resImages.back().rdb;

		return nullptr;													// This return point should never be reached (fail safe)
	}

	bool i_UnloadImage(const uint8_t imageID)
	{
		for (auto& i : i_resImages)										// Check for existing imageID in our image list
			if (i.ID == imageID)
			{
				if (i.bIsLoaded)										// Delete the Sprite and Decal data if the image is already loaded
				{
					i.rdb.Sprite()->pColData.clear();
					i.rdb.Sprite()->pColData.shrink_to_fit();
					i.rdb.Decal()->Update();
					i.bIsLoaded = false;
				}

				return true;
			}

		return false;													// No image found in the list
	}

	bool i_RemoveImage(const uint8_t imageID)
	{
		std::deque<resImage>::iterator it;
		it = std::find(ResImages().begin(), ResImages().end(), imageID);

		if (it != ResImages().end())
		{
			//ResImages().erase(it);
			return true;
		}

		return false;
	}

	bool i_IsLoaded(const uint8_t imageID)
	{
		for (auto& i : i_resImages)
			if (i.ID == imageID && i.bIsLoaded)
				return true;

		return false;
	}

public:
	static std::deque<resImage>&	ResImages()															{ return GetInstance().i_resImages; }
	static std::deque<resSound>&	ResSounds()															{ return GetInstance().i_resSounds; }

	static olc::Renderable*			Image(const uint8_t imageID, const std::string imageFileName = "")	{ return GetInstance().i_Image(imageID, imageFileName); }
	static bool						UnloadImage(const uint8_t imageID)									{ return GetInstance().i_UnloadImage(imageID); }
	static bool						RemoveImage(const uint8_t imageID)									{ return GetInstance().i_RemoveImage(imageID); }
	static bool						IsLoaded(const uint8_t imageID)										{ return GetInstance().i_IsLoaded(imageID); }
};
#endif		//RES_MAN_SINGLE_INSTANCE


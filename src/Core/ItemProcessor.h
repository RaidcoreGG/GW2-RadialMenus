///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// Name         :  ItemProcessor.h
/// Description  :  Handles execution of items and renders an interactable widget.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef ITEMPROCESSOR_H
#define ITEMPROCESSOR_H

#include <mutex>
#include <condition_variable>

#include "RadialItem.h"

class CItemProcessor
{
	public:
	bool                    IsVisible          = true;
	bool                    IsEditing          = false;

	CItemProcessor();
	~CItemProcessor();

	void Render();

	void QueueItem(RadialItem* aItem);

	private:
	std::mutex              Mutex;
	
	std::mutex              QueueMutex;
	RadialItem*             QueuedItem         = nullptr;

	std::condition_variable ConVar;
	bool                    KillThread         = false;
	std::mutex              ThreadMutex;
	std::thread             ProcessorThread;

	std::mutex              ActionsMutex;
	RadialItem              ActiveItem         = {};
	int                     ElapsedTime        = 0;
	bool                    IsCancelled        = false;
	bool                    IsExecuting        = false;

	Texture*                WidgetBase         = nullptr;
	Texture*                WidgetFallbackIcon = nullptr;

	void Process();

	void AwaitActivation();
	void ExecuteActiveActions();
	void DestroyActiveActions();
};

#endif

#include "Shared.h"

HMODULE SelfModule					= nullptr;
AddonAPI* APIDefs					= nullptr;
AddonDefinition AddonDef			= {};

Mumble::Data* MumbleLink			= nullptr;
Mumble::Identity* MumbleIdentity	= nullptr;
NexusLinkData* NexusLink			= nullptr;

void* RadialAPI						= nullptr;

Texture* MW_BASE = nullptr;
Texture* MW_DIVIDER = nullptr;
Texture* MW_2_SELECTOR = nullptr;
Texture* MW_3_SELECTOR = nullptr;
Texture* MW_4_SELECTOR = nullptr;
Texture* MW_5_SELECTOR = nullptr;
Texture* MW_6_SELECTOR = nullptr;
Texture* MW_7_SELECTOR = nullptr;
Texture* MW_8_SELECTOR = nullptr;
Texture* MW_9_SELECTOR = nullptr;
Texture* MW_10_SELECTOR = nullptr;
Texture* MW_11_SELECTOR = nullptr;
Texture* MW_12_SELECTOR = nullptr;

Texture* MWS_BASE = nullptr;
Texture* MWS_DIVIDER = nullptr;
Texture* MWS_2_SELECTOR = nullptr;
Texture* MWS_3_SELECTOR = nullptr;
Texture* MWS_4_SELECTOR = nullptr;
Texture* MWS_5_SELECTOR = nullptr;
Texture* MWS_6_SELECTOR = nullptr;
Texture* MWS_7_SELECTOR = nullptr;
Texture* MWS_8_SELECTOR = nullptr;
